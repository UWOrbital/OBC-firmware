#include "fec.h"
#include "ax25.h"
#include "encode_telemetry.h"
#include "send_telemetry.h"
#include "cc1120_txrx.h"

#include "telemetry_fs_utils.h"
#include "telemetry_manager.h"
#include "telemetry_pack.h"

#include "obc_task_config.h"
#include "obc_logging.h"
#include "obc_errors.h"
#include "obc_fs_utils.h"
#include "comms_manager.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_queue.h>
#include <os_task.h>

#include <sys_common.h>
#include <gio.h>

static TaskHandle_t telemEncodeTaskHandle = NULL;
static StaticTask_t telemEncodeTaskBuffer;
static StackType_t telemEncodeTaskStack[COMMS_TELEM_ENCODE_STACK_SIZE];

#define COMMS_TELEM_ENCODE_QUEUE_LENGTH 2U
#define COMMS_TELEM_ENCODE_QUEUE_ITEM_SIZE sizeof(comms_event_t)  // Size of the telemetry batch ID
#define COMMS_TELEM_ENCODE_QUEUE_RX_WAIT_PERIOD portMAX_DELAY
#define COMMS_TELEM_ENCODE_QUEUE_TX_WAIT_PERIOD portMAX_DELAY

static QueueHandle_t telemEncodeQueueHandle = NULL;
static StaticQueue_t telemEncodeQueue;
static uint8_t telemEncodeQueueStack[COMMS_TELEM_ENCODE_QUEUE_LENGTH * COMMS_TELEM_ENCODE_QUEUE_ITEM_SIZE];

/**
 * @brief Puts telemetry data through OSI model layers and queues into the CC1120 transmit queue
 *
 * @param pvParameters - NULL
 */
static void vTelemEncodeTask(void *pvParameters);

/**
 * @brief Sends data from a telemetry buffer to the CC1120 transmit queue
 *
 * @param telemetryDataBuffer - Pointer to the telemetry_data_t buffer
 * @param numTelemetryData - Number of telemetry data points in the buffer
 * @return obc_error_code_t
 */
static obc_error_code_t sendTelemetryBuffer(telemetry_data_t *telemetryDataBuffer, uint8_t numTelemetryData);

/**
 * @brief Reads chunks of telemetry out of a file and sends it into the CC1120 transmit queue
 *
 * @param telemetryBatchId - ID of the telemetry batch to send
 * @return obc_error_code_t - OBC_ERR_CODE_SUCCESS if all telemetry data was sent successfully
 */
static obc_error_code_t sendTelemetryFile(uint32_t telemetryBatchId);

/**
 * @brief Get the file descriptor object for the telemetry file
 *
 * @param telemetryBatchId - ID of the telemetry batch to send
 * @param fd - Pointer to the file descriptor to be set
 * @return obc_error_code_t
 */
static obc_error_code_t getFileDescriptor(uint32_t telemetryBatchId, int32_t *fd);

/**
 * @brief Sends a telemetry packet, applying FEC and AX.25 framing
 *
 * @param telemPacket - A complete telemetry packet of size 223B
 * @return obc_error_code_t
 */
static obc_error_code_t sendTelemetryPacket(packed_telem_packet_t *telemPacket);

/**
 * @brief Either sends a single piece of telemetry or packs it into the current telemetry packet
 *
 * @param singleTelem - A single piece of telemetry_data_t
 * @param telemPacket - A complete telemetry packet of size 223B
 * @param telemPacketOffset - Pointer to number of bytes already filled in telemPacket
 * @return obc_error_code_t
 */
static obc_error_code_t sendOrPackNextTelemetry(telemetry_data_t *singleTelem, packed_telem_packet_t *telemPacket,
                                                size_t *telemPacketOffset);

/**
 * @brief Initializes the telemetry encoding task and queue
 * @param pvParameters parameters to be passed into the created task
 *
 */
void initTelemEncodeTask(void *pvParameters) {
  ASSERT((telemEncodeTaskStack != NULL) && (&telemEncodeTaskBuffer != NULL));

  if (telemEncodeTaskHandle == NULL) {
    telemEncodeTaskHandle =
        xTaskCreateStatic(vTelemEncodeTask, COMMS_TELEM_ENCODE_TASK_NAME, COMMS_TELEM_ENCODE_STACK_SIZE, pvParameters,
                          COMMS_TELEM_ENCODE_TASK_PRIORITY, telemEncodeTaskStack, &telemEncodeTaskBuffer);
  }

  if (telemEncodeQueueHandle == NULL) {
    telemEncodeQueueHandle = xQueueCreateStatic(COMMS_TELEM_ENCODE_QUEUE_LENGTH, COMMS_TELEM_ENCODE_QUEUE_ITEM_SIZE,
                                                telemEncodeQueueStack, &telemEncodeQueue);
  }
}

/**
 * @brief Sends downlink data to encoding task queue
 *
 * @param queueMsg - Includes command ID, and either a telemetry batch ID or a telemetry_data_t array
 * @return obc_error_code_t - OBC_ERR_CODE_SUCCESS if the telemetry batch ID was successfully sent to the queue
 */
obc_error_code_t sendToDownlinkQueue(comms_event_t *queueMsg) {
  ASSERT(telemEncodeQueueHandle != NULL);

  if (xQueueSend(telemEncodeQueueHandle, (void *)queueMsg, COMMS_TELEM_ENCODE_QUEUE_TX_WAIT_PERIOD) == pdPASS) {
    return OBC_ERR_CODE_SUCCESS;
  }

  return OBC_ERR_CODE_QUEUE_FULL;
}

/**
 * @brief Puts telemetry data through OSI model layers and queues into the CC1120 transmit queue
 *
 * @param pvParameters - NULL
 */
static void vTelemEncodeTask(void *pvParameters) {
  obc_error_code_t errCode;
  SemaphoreHandle_t cc1120Mutex = *((SemaphoreHandle_t *)pvParameters);

  while (1) {
    comms_event_t queueMsg;

    // Wait for a telemetry downlink event
    if (xQueueReceive(telemEncodeQueueHandle, &queueMsg, COMMS_TELEM_ENCODE_QUEUE_RX_WAIT_PERIOD) != pdPASS) {
      // TODO: Handle this if necessary
      continue;
    }

    switch (queueMsg.eventID) {
      case DOWNLINK_TELEMETRY_FILE:
        if (xSemaphoreTake(cc1120Mutex, CC1120_MUTEX_TIMEOUT) != pdTRUE) {
          LOG_ERROR_CODE(OBC_ERR_CODE_MUTEX_TIMEOUT);
          break;
        }
        LOG_IF_ERROR_CODE(sendTelemetryFile(queueMsg.telemetryBatchId));
        // wait for TX FIFO to be emptied before unlocking the mutex
        txFifoEmptyCheckBlocking();
        xSemaphoreGive(cc1120Mutex);
        break;
      case DOWNLINK_DATA_BUFFER:
        if (xSemaphoreTake(cc1120Mutex, CC1120_MUTEX_TIMEOUT) != pdTRUE) {
          LOG_ERROR_CODE(OBC_ERR_CODE_MUTEX_TIMEOUT);
          break;
        }
        LOG_IF_ERROR_CODE(
            sendTelemetryBuffer(queueMsg.telemetryDataBuffer.telemData, queueMsg.telemetryDataBuffer.bufferSize));
        // wait for TX FIFO to be emptied before unlocking the mutex
        txFifoEmptyCheckBlocking();
        xSemaphoreGive(cc1120Mutex);
        break;
      default:
        LOG_ERROR_CODE(OBC_ERR_CODE_INVALID_ARG);
        break;
    }
  }
}

/**
 * @brief Sends data from a telemetry buffer to the CC1120 transmit queue
 *
 * @param telemetryDataBuffer - Pointer to the telemetry_data_t buffer
 * @param numTelemetryData - Number of telemetry data points in the buffer
 * @return obc_error_code_t
 */
static obc_error_code_t sendTelemetryBuffer(telemetry_data_t *telemetryDataBuffer, uint8_t numTelemetryData) {
  obc_error_code_t errCode;

  if (telemetryDataBuffer == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  if (numTelemetryData == 0) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  // Initialize important variables related to packing and queueing the telemetry to be sen
  packed_telem_packet_t telemPacket = {0};  // Holds 223B of "raw" telemetry data.
                                            // Zero initialized because telem IDs of 0 are ignored at the ground station
  size_t telemPacketOffset = 0;             // Number of bytes filled in telemPacket

  // Loop through all telemetry data in the buffer
  for (uint8_t i = 0; i < numTelemetryData; i++) {
    RETURN_IF_ERROR_CODE(sendOrPackNextTelemetry(telemetryDataBuffer + i, &telemPacket, &telemPacketOffset));
  }

  // Send the last packet if it is not empty
  if (telemPacketOffset == 0) return OBC_ERR_CODE_SUCCESS;

  RETURN_IF_ERROR_CODE(sendTelemetryPacket(&telemPacket));

  return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief Reads chunks of telemetry out of a file and sends it into the CC1120 transmit queue
 *
 * @param telemetryBatchId - ID of the telemetry batch to send
 * @return obc_error_code_t - OBC_ERR_CODE_SUCCESS if all telemetry data was sent successfully
 */
static obc_error_code_t sendTelemetryFile(uint32_t telemetryBatchId) {
  obc_error_code_t errCode;

  // Get the file descriptor for the telemetry file
  int32_t fd;
  RETURN_IF_ERROR_CODE(getFileDescriptor(telemetryBatchId, &fd));

  // Initialize important variables related to packing and queueing the telemetry to be sent
  telemetry_data_t singleTelem;  // Holds a single piece of telemetry from getNextTelemetry()

  packed_telem_packet_t telemPacket = {0};  // Holds 223B of "raw" telemetry data.
                                            // Zero initialized because telem IDs of 0 are ignored at the ground station
  size_t telemPacketOffset = 0;             // Number of bytes filled in telemPacket

  // Read a single piece of telemetry from the file
  while ((errCode = readNextTelemetryFromFile(fd, &singleTelem)) == OBC_ERR_CODE_SUCCESS) {
    errCode = sendOrPackNextTelemetry(&singleTelem, &telemPacket, &telemPacketOffset);
    if (errCode != OBC_ERR_CODE_SUCCESS) {
      LOG_ERROR_CODE(errCode);
      RETURN_IF_ERROR_CODE(closeTelemetryFile(fd));
      return errCode;
    }
  }

  if (errCode == OBC_ERR_CODE_REACHED_EOF) {
    LOG_DEBUG("Reached end of telemetry file");
    errCode = OBC_ERR_CODE_SUCCESS;
  } else {
    LOG_ERROR_CODE(errCode);  // If the error wasn't an EOF error, return
    RETURN_IF_ERROR_CODE(closeTelemetryFile(fd));
    return errCode;
  }

  // If there's no data left to send, return
  if (telemPacketOffset == 0) return OBC_ERR_CODE_SUCCESS;

  errCode = sendTelemetryPacket(&telemPacket);
  if (errCode != OBC_ERR_CODE_SUCCESS) {
    LOG_ERROR_CODE(errCode);
    RETURN_IF_ERROR_CODE(closeTelemetryFile(fd));
    return errCode;
  }

  // Close telemetry file
  RETURN_IF_ERROR_CODE(closeTelemetryFile(fd));

  return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief Get the file descriptor object for the telemetry file
 *
 * @param telemetryBatchId - ID of the telemetry batch to send
 * @param fd - Pointer to the file descriptor to be set
 * @return obc_error_code_t
 */
static obc_error_code_t getFileDescriptor(uint32_t telemetryBatchId, int32_t *fd) {
  obc_error_code_t errCode;

  if (fd == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  // Open the telemetry file
  RETURN_IF_ERROR_CODE(openTelemetryFileRO(telemetryBatchId, fd));

  // Print the telemetry file size for debugging
  size_t fileSize;
  errCode = getFileSize(*fd, &fileSize);
  if (errCode != OBC_ERR_CODE_SUCCESS) {
    LOG_ERROR_CODE(errCode);
    closeTelemetryFile(*fd);
    return errCode;
  }
  LOG_DEBUG("Sending telemetry file with size: %lu", fileSize);

  // Print telemetry file name
  char fileName[TELEMETRY_FILE_PATH_MAX_LENGTH] = {0};
  errCode = constructTelemetryFilePath(telemetryBatchId, fileName, TELEMETRY_FILE_PATH_MAX_LENGTH);
  if (errCode != OBC_ERR_CODE_SUCCESS) {
    LOG_ERROR_CODE(errCode);
    closeTelemetryFile(*fd);
    return errCode;
  }

  LOG_DEBUG("Sending telemetry file with name: %s", fileName);

  return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief Either sends a single piece of telemetry or packs it into the current telemetry packet
 *
 * @param singleTelem - A single piece of telemetry_data_t
 * @param telemPacket - A complete telemetry packet of size 223B
 * @param telemPacketOffset - Pointer to number of bytes already filled in telemPacket
 * @return obc_error_code_t
 */
static obc_error_code_t sendOrPackNextTelemetry(telemetry_data_t *singleTelem, packed_telem_packet_t *telemPacket,
                                                size_t *telemPacketOffset) {
  obc_error_code_t errCode;

  LOG_DEBUG("Sending telemetry: %u", singleTelem->id);

  uint8_t packedSingleTelem[MAX_TELEMETRY_DATA_SIZE];  // Holds a serialized version of the current piece of telemetry
  uint32_t packedSingleTelemSize = 0;                  // Size of the packed single telemetry

  // Pack the single telemetry into a uint8_t array
  RETURN_IF_ERROR_CODE(packTelemetry(singleTelem, packedSingleTelem, sizeof(packedSingleTelem) / sizeof(uint8_t),
                                     &packedSingleTelemSize));

  // If the single telemetry is too large to continue adding to the telemPacket, send the telemPacket
  if ((*telemPacketOffset) + packedSingleTelemSize > PACKED_TELEM_PACKET_SIZE) {
    RETURN_IF_ERROR_CODE(sendTelemetryPacket(telemPacket));
    // Reset the packedTelem struct and offset
    *telemPacket = (packed_telem_packet_t){0};
    *telemPacketOffset = 0;
  }

  // Copy the telemetry data into the packedTelem struct
  memcpy(telemPacket->data + (*telemPacketOffset), packedSingleTelem, packedSingleTelemSize);
  *telemPacketOffset += packedSingleTelemSize;

  return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief Sends a telemetry packet, applying FEC and AX.25 framing
 *
 * @param telemPacket - A complete telemetry packet of size 223B
 * @return obc_error_code_t
 */
static obc_error_code_t sendTelemetryPacket(packed_telem_packet_t *telemPacket) {
  obc_error_code_t errCode;

  packed_rs_packet_t fecPkt = {0};  // Holds a 255B RS packet
  unstuffed_ax25_i_frame_t unstuffedAx25Pkt = {0};
  packed_ax25_i_frame_t ax25Pkt = {0};  // Holds an AX.25 packet

  // Perform AX.25 framing
  RETURN_IF_ERROR_CODE(ax25SendIFrame(telemPacket->data, RS_DECODED_SIZE, &unstuffedAx25Pkt, &groundStationCallsign));

  // Apply Reed Solomon FEC
  RETURN_IF_ERROR_CODE(rsEncode(unstuffedAx25Pkt.data + AX25_INFO_FIELD_POSITION, &fecPkt));

  memcpy(unstuffedAx25Pkt.data + AX25_INFO_FIELD_POSITION, fecPkt.data, RS_ENCODED_SIZE);

  RETURN_IF_ERROR_CODE(ax25Stuff(unstuffedAx25Pkt.data, unstuffedAx25Pkt.length, ax25Pkt.data, &ax25Pkt.length));

  ax25Pkt.data[0] = AX25_FLAG;
  ax25Pkt.data[ax25Pkt.length - 1] = AX25_FLAG;

  // Send into CC1120 transmit queue
  RETURN_IF_ERROR_CODE(sendToCC1120TransmitQueue(&ax25Pkt));

  return OBC_ERR_CODE_SUCCESS;
}
