#include "payload_manager.h"
#include "camera_control.h"
#include "camera_fs_utils.h"
#include "downlink_encoder.h"
#include "obc_errors.h"
#include "obc_logging.h"
#include "obc_scheduler_config.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_queue.h>
#include <os_task.h>

#include <sys_common.h>
#include <gio.h>

/* Number of image bytes read from the camera FIFO and written to the SD card at a time */
#define IMAGE_CHUNK_SIZE 64U

/* Capture completion polling config */
#define CAPTURE_DONE_POLL_PERIOD pdMS_TO_TICKS(10)
#define CAPTURE_DONE_MAX_POLLS 300U

static QueueHandle_t payloadQueueHandle = NULL;
static StaticQueue_t payloadQueue;
static uint8_t payloadQueueStack[PAYLOAD_MANAGER_QUEUE_LENGTH * PAYLOAD_MANAGER_QUEUE_ITEM_SIZE];

void obcTaskInitPayloadMgr(void) {
  ASSERT((payloadQueueStack != NULL) && (&payloadQueue != NULL));
  if (payloadQueueHandle == NULL) {
    payloadQueueHandle = xQueueCreateStatic(PAYLOAD_MANAGER_QUEUE_LENGTH, PAYLOAD_MANAGER_QUEUE_ITEM_SIZE,
                                            payloadQueueStack, &payloadQueue);
  }

  initImageFileMutex();
}

obc_error_code_t sendToPayloadQueue(payload_event_t *event) {
  ASSERT(payloadQueueHandle != NULL);

  if (event == NULL) return OBC_ERR_CODE_INVALID_ARG;

  if (xQueueSend(payloadQueueHandle, (void *)event, PAYLOAD_MANAGER_QUEUE_TX_WAIT_PERIOD) == pdPASS) {
    return OBC_ERR_CODE_SUCCESS;
  }
  return OBC_ERR_CODE_QUEUE_FULL;
}

/**
 * @brief Capture an image with the selected camera and store it on the SD card at IMAGE_FILE_PATH.
 *
 * @param cameraID Camera ID of camera
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if successful, otherwise error code
 */
static obc_error_code_t captureImageToSD(camera_id_t cameraID) {
  obc_error_code_t errCode;

  RETURN_IF_ERROR_CODE(initCamera(cameraID));
  RETURN_IF_ERROR_CODE(camConfigureSensor());
  RETURN_IF_ERROR_CODE(startImageCapture(cameraID));

  uint32_t polls = 0;
  while (1) {
    errCode = isCaptureDone(cameraID);
    if (errCode == OBC_ERR_CODE_CAMERA_CAPTURE_COMPLETE) {
      break;
    }
    if (errCode != OBC_ERR_CODE_CAMERA_CAPTURE_INCOMPLETE) {
      return errCode;
    }
    if (polls >= CAPTURE_DONE_MAX_POLLS) {
      return OBC_ERR_CODE_CAMERA_CAPTURE_INCOMPLETE;
    }
    polls++;
    vTaskDelay(CAPTURE_DONE_POLL_PERIOD);
  }

  RETURN_IF_ERROR_CODE(mkImageDir());

  int32_t imageFileId = -1;
  RETURN_IF_ERROR_CODE(openImageFileWrite(&imageFileId));

  // Read the image out of the camera FIFO and onto the SD card in chunks small
  // enough to fit on the task stack
  uint8_t imageChunk[IMAGE_CHUNK_SIZE];
  obc_error_code_t readCode;
  do {
    size_t bytesRead = 0;
    readCode = readImage(cameraID, imageChunk, IMAGE_CHUNK_SIZE, &bytesRead);
    if ((readCode != OBC_ERR_CODE_SUCCESS) && (readCode != OBC_ERR_CODE_CAMERA_IMAGE_READ_INCOMPLETE)) {
      closeImageFile(imageFileId);
      return readCode;
    }

    if (bytesRead > 0) {
      errCode = writeImageToFile(imageFileId, imageChunk, bytesRead);
      if (errCode != OBC_ERR_CODE_SUCCESS) {
        closeImageFile(imageFileId);
        return errCode;
      }
    }
  } while (readCode == OBC_ERR_CODE_CAMERA_IMAGE_READ_INCOMPLETE);

  RETURN_IF_ERROR_CODE(closeImageFile(imageFileId));

  return OBC_ERR_CODE_SUCCESS;
}

void obcTaskFunctionPayloadMgr(void *pvParameters) {
  obc_error_code_t errCode;

  ASSERT(payloadQueueHandle != NULL);

  while (1) {
    payload_event_t queueMsg;
    if (xQueueReceive(payloadQueueHandle, &queueMsg, PAYLOAD_MANAGER_QUEUE_RX_WAIT_PERIOD) == pdTRUE) {
      switch (queueMsg.eventID) {
        case PAYLOAD_MANAGER_NULL_EVENT_ID:
          break;

        case PAYLOAD_CAPTURE_IMAGE_EVENT_ID: {
          LOG_IF_ERROR_CODE(captureImageToSD(PRIMARY));
          obc_error_code_t captureCode = errCode;

          // Always attempt to put the camera back into standby, even if the capture failed
          LOG_IF_ERROR_CODE(standbyCamera(PRIMARY));

          // The image file is only complete once captureImageToSD has succeeded, so this is
          // the only place a downlink of the image may be triggered from
          if (captureCode == OBC_ERR_CODE_SUCCESS) {
            encode_event_t encodeEvent = {.eventID = DOWNLINK_IMAGE_FILE};
            LOG_IF_ERROR_CODE(sendToDownlinkEncodeQueue(&encodeEvent));
          }
          break;
        }

        case SECONDARY_PAYLOAD_MANAGER_EVENT_ID:
          // ADD SECONDARY PAYLOAD COMMAND HANDLER
          break;
      }
    }
  }
}
