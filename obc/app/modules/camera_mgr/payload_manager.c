#include "payload_manager.h"
#include "camera_control.h"
#include "downlink_encoder.h"
#include "obc_errors.h"
#include "obc_logging.h"
#include "obc_scheduler_config.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_queue.h>
#include <os_semphr.h>
#include <os_task.h>

#include <sys_common.h>
#include <gio.h>

static QueueHandle_t payloadQueueHandle = NULL;
static StaticQueue_t payloadQueue;
static uint8_t payloadQueueStack[PAYLOAD_MANAGER_QUEUE_LENGTH * PAYLOAD_MANAGER_QUEUE_ITEM_SIZE];

// Signalled by setPayloadManagerDownlinkReady() (from the CMD_CAPTURE_IMAGE callback) and
// consumed by the payload manager task, which then captures + downlinks one image.
static SemaphoreHandle_t payloadDownlinkReady = NULL;
static StaticSemaphore_t payloadDownlinkReadyBuffer;

// Which camera the pending capture request targets. Set alongside the semaphore give.
static volatile uint8_t requestedCameraId = 0;  // PRIMARY

void obcTaskInitPayloadMgr(void) {
  ASSERT((payloadQueueStack != NULL) && (&payloadQueue != NULL));
  if (payloadQueueHandle == NULL) {
    payloadQueueHandle = xQueueCreateStatic(PAYLOAD_MANAGER_QUEUE_LENGTH, PAYLOAD_MANAGER_QUEUE_ITEM_SIZE,
                                            payloadQueueStack, &payloadQueue);
  }

  if (payloadDownlinkReady == NULL) {
    payloadDownlinkReady = xSemaphoreCreateBinaryStatic(&payloadDownlinkReadyBuffer);
  }
}

obc_error_code_t sendToPayloadQueue(payload_event_t *event) {
  ASSERT(payloadQueueHandle != NULL);

  if (event == NULL) return OBC_ERR_CODE_INVALID_ARG;

  if (xQueueSend(payloadQueueHandle, (void *)event, PAYLOAD_MANAGER_QUEUE_TX_WAIT_PERIOD) == pdPASS) {
    return OBC_ERR_CODE_SUCCESS;
  }
  return OBC_ERR_CODE_QUEUE_FULL;
}

obc_error_code_t setPayloadManagerDownlinkReady(uint8_t cameraId) {
  if (payloadDownlinkReady == NULL) {
    return OBC_ERR_CODE_INVALID_STATE;
  }

  requestedCameraId = cameraId;

  // Binary semaphore: give fails only if a capture is already pending. Treat that as
  // "already requested" rather than a hard error.
  if (xSemaphoreGive(payloadDownlinkReady) != pdPASS) {
    return OBC_ERR_CODE_SEMAPHORE_FULL;
  }

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

        case SECONDARY_PAYLOAD_MANAGER_EVENT_ID:
          // ADD SECONDARY PAYLOAD COMMAND HANDLER
          break;
      }
    }

    // Handle a pending image-capture request, if any.
    if (xSemaphoreTake(payloadDownlinkReady, 0) != pdPASS) {
      continue;
    }

    camera_id_t cameraID = (camera_id_t)requestedCameraId;

    // Capture the image into the Arducam's on-board FIFO. The whole JPEG never lives in
    // OBC RAM - the downlink encoder streams it straight out of the FIFO afterwards.
    LOG_IF_ERROR_CODE(captureImage(cameraID));
    if (errCode != OBC_ERR_CODE_SUCCESS) {
      continue;  // capture failed; nothing to downlink
    }

    // Hand off to the downlink encoder, which reads the FIFO in RS-sized chunks and
    // pushes each through sendPacket() (same path as telemetry/command responses).
    encode_event_t encodeEvent = {.eventID = DOWNLINK_IMAGE, .cameraId = (uint8_t)cameraID};
    LOG_IF_ERROR_CODE(sendToDownlinkEncodeQueue(&encodeEvent));
  }
}
