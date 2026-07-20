#include "payload_manager.h"
#include "obc_errors.h"
#include "obc_scheduler_config.h"
#include "comms_manager.h"
#include "downlink_encoder.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_queue.h>
#include <os_semphr.h>
#include <os_task.h>

#include <sys_common.h>
#include <gio.h>

#ifdef CONFIG_SDCARD
/**
 * @brief Check if it's time to downlink the payload.
 * @return bool True if it's time to downlink the payload, false otherwise
 */
static bool checkPayloadDownlink(void);
#endif  // CONFIG_SDCARD

static QueueHandle_t payloadQueueHandle = NULL;
static StaticQueue_t payloadQueue;
static uint8_t payloadQueueStack[PAYLOAD_MANAGER_QUEUE_LENGTH * PAYLOAD_MANAGER_QUEUE_ITEM_SIZE];

static SemaphoreHandle_t payloadDownlinkReady = NULL;
static StaticSemaphore_t payloadDownlinkReadyBuffer;

void obcTaskInitPayloadMgr(void) {
  ASSERT((payloadQueueStack != NULL) && (&payloadQueue != NULL));
  if (payloadQueueHandle == NULL) {
    payloadQueueHandle = xQueueCreateStatic(PAYLOAD_MANAGER_QUEUE_LENGTH, PAYLOAD_MANAGER_QUEUE_ITEM_SIZE,
                                            payloadQueueStack, &payloadQueue);
  }

  ASSERT(&payloadDownlinkReadyBuffer != NULL);
  payloadDownlinkReady = xSemaphoreCreateBinaryStatic(&payloadDownlinkReadyBuffer);
}

obc_error_code_t sendToPayloadQueue(payload_event_t *event) {
  ASSERT(payloadQueueHandle != NULL);

  if (event == NULL) return OBC_ERR_CODE_INVALID_ARG;

  if (xQueueSend(payloadQueueHandle, (void *)event, PAYLOAD_MANAGER_QUEUE_TX_WAIT_PERIOD) == pdPASS) {
    return OBC_ERR_CODE_SUCCESS;
  }
  return OBC_ERR_CODE_QUEUE_FULL;
}

void obcTaskFunctionPayloadMgr(void *pvParameters) {
#ifdef CONFIG_SDCARD
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

    if (!checkPayloadDownlink()) {
      continue;
    }

    // Close whatever file your writing to 
    encode_event_t encodeEvent = {.eventID = DOWNLINK_TELEMETRY_FILE, .telemetryBatchId = telemetryBatchId};

    LOG_IF_ERROR_CODE(sendToDownlinkEncodeQueue(&encodeEvent));
    if (errCode != OBC_ERR_CODE_SUCCESS) {
      // TODO: Handle errors 
    }

    // Open new file if necessary for image tho, that logic should probably be elsewhere

  }
#else
  vTaskSuspend(NULL);
#endif  // CONFIG_SDCARD
}


#ifdef CONFIG_SDCARD
static bool checkPayloadDownlink(void) { return xSemaphoreTake(payloadDownlinkReady, 0) == pdPASS; }
#endif  // CONFIG_SDCARD

obc_error_code_t setPayloadManagerDownlinkReady(void) {
  if (xSemaphoreGive(payloadDownlinkReady) != pdPASS) {
    return OBC_ERR_CODE_SEMAPHORE_FULL;
  }

  return OBC_ERR_CODE_SUCCESS;
}
