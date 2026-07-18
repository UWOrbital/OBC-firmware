/**
 * ============================================================================
 * ARDUCAM PAYLOAD PIPELINE DEMO (SD-card image path, no camera required)
 * ============================================================================
 *
 * This example demonstrates the full "image on SD card" payload pipeline using
 * STUB DATA instead of a real ArduCam, so it can run on any board with an SD
 * card. It exercises the exact same code the flight software uses
 * (camera_fs_utils + the packet format used by the downlink encoder).
 *
 * -------------------------- HOW THE REAL SYSTEM WORKS ----------------------
 *
 * 1) UPLINK: the ground station (GS) sends CMD_CAPTURE_IMAGE. It arrives over
 *    the radio (CC1120) or UART, is AX.25-deframed, RS-decoded and AES-decrypted
 *    by the uplink decoder, and lands in the command manager's queue.
 *
 * 2) COMMAND CALLBACK (command_callbacks.c): captureImageCmdCallback() does NOT
 *    touch the camera. It only queues a PAYLOAD_CAPTURE_IMAGE_EVENT_ID event to
 *    the payload manager and returns, so the command manager task is never
 *    blocked by a slow capture. The command response only means "capture
 *    request queued".
 *
 * 3) PAYLOAD MANAGER (payload_manager.c): its task receives the event and runs
 *    captureImageToSD():
 *      - initCamera / camConfigureSensor / startImageCapture
 *      - polls isCaptureDone() every 10 ms (bounded, no busy-spin)
 *      - streams the ArduCam FIFO to the SD card in 64-byte chunks via
 *        camera_fs_utils, so the full image (much larger than any task stack)
 *        is never held in RAM
 *      - the image lives at IMAGE_FILE_PATH ("/arducam/image.jpg"), truncated
 *        and rewritten on every capture
 *    On success (and only then) it queues a DOWNLINK_IMAGE_FILE event to the
 *    downlink encoder: the downlink can therefore never start before the image
 *    is fully written.
 *
 * 4) CONCURRENCY: camera_fs_utils holds a mutex for an entire open -> close
 *    session (openImageFileWrite/openImageFileRO take it, closeImageFile
 *    releases it). This prevents the one remaining race: a NEW capture
 *    truncating the file while a still-running downlink is reading it.
 *
 * 5) DOWNLINK ENCODER (downlink_encoder.c): sendImageFile() opens the file
 *    read-only, then sends:
 *      - packet 0: a cmd-response packet whose 4-byte payload is the total
 *        image size (little-endian) -> tells the GS how many bytes to expect
 *      - packets 1..N: cmd-response packets each carrying up to 220 bytes of
 *        raw image data (the last one may be short)
 *    Each 223-byte packet then gets Reed-Solomon FEC (-> 255 B) and AX.25
 *    framing before hitting the radio/UART.
 *
 *    CMD-RESPONSE PACKET LAYOUT (223 bytes total, before FEC/AX.25):
 *      byte 0   : cmdId   (CMD_CAPTURE_IMAGE)
 *      byte 1   : errCode (CMD_RESPONSE_SUCCESS)
 *      byte 2   : dataLen (0..220)
 *      byte 3.. : dataLen bytes of payload, zero-padded to 220
 *
 * ----------------------------- WHAT THIS DEMO DOES -------------------------
 *
 *  STEP 1 (stands in for the payload manager write path):
 *    - init the image file mutex and mount the filesystem
 *    - generate a recognizable ASCII stub "image" and write it to
 *      /arducam/image.jpg in 64-byte chunks, exactly like captureImageToSD()
 *      (only the camera FIFO reads are replaced by the stub buffer)
 *
 *  STEP 2 (stands in for the downlink encoder + ground station):
 *    - open the file read-only, get its size
 *    - build the same packets sendImageFile() builds (size packet + 220-byte
 *      data packets) using the real packCmdResponse()
 *    - "transmit" each packet by parsing it the way the GS would (read the
 *      3-byte header, take dataLen payload bytes) and reassembling the image
 *    - verify the reassembled bytes match the stub byte-for-byte and print
 *      the recovered text over UART
 *
 * ------------------------------- FUTURE WORK -------------------------------
 *  - GS side: parse the size packet + data packets and reassemble image.jpg
 *    (this demo's STEP 2 parsing loop is the reference implementation)
 *  - Optionally add a dedicated CMD_DOWNLINK_IMAGE ground command (in the
 *    interfaces submodule) instead of auto-downlinking after every capture
 *  - Persist multiple images / add image metadata (timestamp, camera ID)
 *  - Run this same pipeline with the real camera by swapping the stub buffer
 *    for readImage() calls (that code already exists in payload_manager.c)
 * ============================================================================
 */

#include "obc_errors.h"
#include "obc_logging.h"
#include "obc_print.h"
#include "obc_reliance_fs.h"
#include "obc_sci_io.h"
#include "obc_spi_io.h"

#include "camera_fs_utils.h"
#include "obc_gs_command_id.h"
#include "obc_gs_commands_response.h"
#include "obc_gs_commands_response_pack.h"
#include "obc_gs_errors.h"
#include "obc_gs_fec.h"

#include <FreeRTOS.h>
#include <os_task.h>

#include <sys_common.h>
#include <sci.h>
#include <spi.h>

#include <string.h>
#include <stdint.h>

#define TASK_STACK_SIZE 2048U
static StaticTask_t taskBuffer;
static StackType_t taskStack[TASK_STACK_SIZE];

/* Stub image config. 1337 is deliberately NOT a multiple of 220 so the demo
 * exercises a short final data packet, and NOT a multiple of 64 so the write
 * side exercises a short final write chunk. */
#define STUB_IMAGE_SIZE 1337U

/* Same chunk size the payload manager uses when streaming the camera FIFO to
 * the SD card (IMAGE_CHUNK_SIZE in payload_manager.c) */
#define WRITE_CHUNK_SIZE 64U

/* The stub image and the GS-side reassembly buffer. Static (not on the task
 * stack) because 2 x 1337 B would eat a large part of the stack. The real
 * system never needs buffers like these - only this demo does, for
 * verification. */
static uint8_t stubImage[STUB_IMAGE_SIZE];
static uint8_t reassembledImage[STUB_IMAGE_SIZE];

/**
 * @brief Fill the stub image buffer with a recognizable repeating ASCII
 * message, so the "downlinked" data is human-readable on the UART console.
 */
static void generateStubImage(void) {
  const char msg[] = "UW ORBITAL ARDUCAM SD-CARD PAYLOAD DEMO! ";
  const size_t msgLen = sizeof(msg) - 1U;  // exclude null terminator
  for (size_t i = 0; i < STUB_IMAGE_SIZE; i++) {
    stubImage[i] = (uint8_t)msg[i % msgLen];
  }
}

/**
 * @brief STEP 1: write the stub image to /arducam/image.jpg in 64-byte chunks.
 *
 * This mirrors captureImageToSD() in payload_manager.c one-for-one; the only
 * difference is that the data comes from stubImage[] instead of the ArduCam
 * FIFO (readImage()).
 */
static obc_error_code_t writeStubImageToSD(void) {
  obc_error_code_t errCode;

  // Idempotent - succeeds if /arducam/ already exists
  RETURN_IF_ERROR_CODE(mkImageDir());

  // Takes the image file mutex and truncates any previous image
  int32_t imageFileId = -1;
  RETURN_IF_ERROR_CODE(openImageFileWrite(&imageFileId));

  size_t bytesWritten = 0;
  while (bytesWritten < STUB_IMAGE_SIZE) {
    size_t chunkLen = STUB_IMAGE_SIZE - bytesWritten;
    if (chunkLen > WRITE_CHUNK_SIZE) {
      chunkLen = WRITE_CHUNK_SIZE;
    }

    errCode = writeImageToFile(imageFileId, &stubImage[bytesWritten], chunkLen);
    if (errCode != OBC_ERR_CODE_SUCCESS) {
      closeImageFile(imageFileId);
      return errCode;
    }
    bytesWritten += chunkLen;
  }

  // Close commits the data and releases the mutex; only after this point may a
  // downlink read the file. In the real system, the payload manager sends the
  // DOWNLINK_IMAGE_FILE event here.
  RETURN_IF_ERROR_CODE(closeImageFile(imageFileId));

  sciPrintf("[payload] wrote %d stub bytes to %s in %d-byte chunks\r\n", (int)bytesWritten, IMAGE_FILE_PATH,
            (int)WRITE_CHUNK_SIZE);
  return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief Parse one cmd-response packet the way the ground station will.
 *
 * This is the GS reference implementation: read the 3-byte header, then take
 * dataLen payload bytes. Data packets are appended to reassembledImage[].
 *
 * @param packet 223-byte packet (RS_DECODED_SIZE), as produced by packCmdResponse
 * @param isSizePacket true for packet 0 (payload = 4-byte image size)
 * @param expectedTotal in/out - set from the size packet, checked by data packets
 * @param reassembledLen in/out - number of image bytes recovered so far
 */
static obc_error_code_t gsParsePacket(const uint8_t *packet, bool isSizePacket, uint32_t *expectedTotal,
                                      size_t *reassembledLen) {
  // Header layout: [0]=cmdId [1]=errCode [2]=dataLen [3..]=payload
  uint8_t cmdId = packet[0];
  uint8_t respErrCode = packet[1];
  uint8_t dataLen = packet[2];
  const uint8_t *payload = &packet[3];

  if (cmdId != (uint8_t)CMD_CAPTURE_IMAGE || respErrCode != (uint8_t)CMD_RESPONSE_SUCCESS) {
    sciPrintf("[gs] unexpected header: cmdId=%d errCode=%d\r\n", cmdId, respErrCode);
    return OBC_ERR_CODE_INVALID_ARG;
  }

  if (isSizePacket) {
    // Packet 0: 4-byte total image size, little-endian
    uint32_t total = 0;
    memcpy(&total, payload, sizeof(total));
    *expectedTotal = total;
    sciPrintf("[gs] size packet: expecting %d image bytes\r\n", (int)total);
    return OBC_ERR_CODE_SUCCESS;
  }

  if (*reassembledLen + dataLen > *expectedTotal || *reassembledLen + dataLen > STUB_IMAGE_SIZE) {
    sciPrintf("[gs] received more data than announced!\r\n");
    return OBC_ERR_CODE_INVALID_ARG;
  }

  memcpy(&reassembledImage[*reassembledLen], payload, dataLen);
  *reassembledLen += dataLen;
  return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief STEP 2: read the image back off the SD card and packetize it exactly
 * like sendImageFile() in downlink_encoder.c, handing each packet to the
 * GS-side parser instead of the radio.
 *
 * In the real system the only difference is that each 223-byte packet goes
 * through sendPacket() (Reed-Solomon FEC -> AX.25 framing -> CC1120/UART)
 * instead of gsParsePacket().
 */
static obc_error_code_t downlinkStubImage(void) {
  obc_error_code_t errCode;

  // Takes the image file mutex: a new "capture" cannot truncate the file under us
  int32_t imageFileId = -1;
  RETURN_IF_ERROR_CODE(openImageFileRO(&imageFileId));

  size_t fileSize = 0;
  errCode = getImageFileSize(imageFileId, &fileSize);
  if (errCode != OBC_ERR_CODE_SUCCESS) {
    closeImageFile(imageFileId);
    return errCode;
  }
  sciPrintf("[downlink] image file is %d bytes\r\n", (int)fileSize);

  uint8_t packetBuffer[RS_DECODED_SIZE] = {0};
  // packCmdResponse always copies a full 220 bytes from responseData, so keep
  // it full-size and zeroed even for short chunks
  uint8_t responseData[CMD_RESPONSE_DATA_MAX_SIZE] = {0};

  uint32_t expectedTotal = 0;
  size_t reassembledLen = 0;
  uint32_t packetCount = 0;

  // ---- Packet 0: total image size ----
  uint32_t imageSize = (uint32_t)fileSize;
  memcpy(responseData, &imageSize, sizeof(imageSize));
  cmd_response_header_t resHeader = {
      .cmdId = CMD_CAPTURE_IMAGE, .errCode = CMD_RESPONSE_SUCCESS, .dataLen = sizeof(imageSize)};
  if (packCmdResponse(&resHeader, packetBuffer, responseData) != OBC_GS_ERR_CODE_SUCCESS) {
    closeImageFile(imageFileId);
    return OBC_ERR_CODE_FAILED_PACK;
  }
  packetCount++;
  errCode = gsParsePacket(packetBuffer, true, &expectedTotal, &reassembledLen);
  if (errCode != OBC_ERR_CODE_SUCCESS) {
    closeImageFile(imageFileId);
    return errCode;
  }

  // ---- Packets 1..N: image data, up to 220 bytes each ----
  while (1) {
    size_t bytesRead = 0;
    memset(responseData, 0, sizeof(responseData));
    errCode = readNextImageChunkFromFile(imageFileId, responseData, CMD_RESPONSE_DATA_MAX_SIZE, &bytesRead);
    if (errCode == OBC_ERR_CODE_REACHED_EOF) {
      errCode = OBC_ERR_CODE_SUCCESS;
      break;
    }
    if (errCode != OBC_ERR_CODE_SUCCESS) {
      break;
    }

    resHeader.dataLen = (uint8_t)bytesRead;
    if (packCmdResponse(&resHeader, packetBuffer, responseData) != OBC_GS_ERR_CODE_SUCCESS) {
      errCode = OBC_ERR_CODE_FAILED_PACK;
      break;
    }
    packetCount++;

    errCode = gsParsePacket(packetBuffer, false, &expectedTotal, &reassembledLen);
    if (errCode != OBC_ERR_CODE_SUCCESS) {
      break;
    }
  }

  closeImageFile(imageFileId);
  if (errCode != OBC_ERR_CODE_SUCCESS) {
    return errCode;
  }

  sciPrintf("[downlink] sent %d packets (1 size + %d data)\r\n", (int)packetCount, (int)(packetCount - 1U));

  // ---- GS-side verification ----
  if (reassembledLen != expectedTotal) {
    sciPrintf("[gs] FAIL: reassembled %d bytes, expected %d\r\n", (int)reassembledLen, (int)expectedTotal);
    return OBC_ERR_CODE_UNKNOWN;
  }
  if (memcmp(reassembledImage, stubImage, STUB_IMAGE_SIZE) != 0) {
    sciPrintf("[gs] FAIL: reassembled data does not match the stub image\r\n");
    return OBC_ERR_CODE_UNKNOWN;
  }

  sciPrintf("[gs] PASS: all %d bytes reassembled correctly\r\n", (int)reassembledLen);
  sciPrintf("[gs] recovered data starts with: \"");
  sciPrintText(reassembledImage, 41, portMAX_DELAY);
  sciPrintf("\"\r\n");

  return OBC_ERR_CODE_SUCCESS;
}

static void payloadPipelineDemoTask(void *pvParameters) {
  obc_error_code_t errCode;

  sciPrintf("\r\n=== ArduCam SD-card payload pipeline demo (stub data) ===\r\n");

  // In the flight build these two happen in obcTaskInitPayloadMgr() and the
  // state manager's filesystem setup respectively
  initImageFileMutex();
  LOG_IF_ERROR_CODE(setupFileSystem());
  if (errCode != OBC_ERR_CODE_SUCCESS) {
    sciPrintf("Filesystem setup failed (%d) - is the SD card inserted?\r\n", errCode);
    while (1)
      ;
  }
  sciPrintf("Filesystem mounted\r\n");

  generateStubImage();

  // STEP 1: payload-manager side (stub capture -> SD card)
  LOG_IF_ERROR_CODE(writeStubImageToSD());
  if (errCode != OBC_ERR_CODE_SUCCESS) {
    sciPrintf("Stub image write failed (%d)\r\n", errCode);
    while (1)
      ;
  }

  // STEP 2: downlink-encoder + GS side (SD card -> packets -> reassembled image)
  LOG_IF_ERROR_CODE(downlinkStubImage());
  if (errCode != OBC_ERR_CODE_SUCCESS) {
    sciPrintf("Stub image downlink failed (%d)\r\n", errCode);
    while (1)
      ;
  }

  sciPrintf("=== Demo complete ===\r\n");
  while (1)
    ;
}

int main(void) {
  // Hardware init: UART for console output, SPI for the SD card
  sciInit();
  spiInit();

  initSciPrint();
  initSpiMutex();

  xTaskCreateStatic(payloadPipelineDemoTask, "payload_demo", TASK_STACK_SIZE, NULL, 1, taskStack, &taskBuffer);

  vTaskStartScheduler();

  while (1)
    ;

  return 0;
}
