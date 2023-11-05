#include "payload_manager.h"
#include "obc_errors.h"
#include "obc_scheduler_config.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_queue.h>
#include <os_task.h>

#include <sys_common.h>
#include <gio.h>

static QueueHandle_t payloadQueueHandle = NULL;
static StaticQueue_t payloadQueue;
static uint8_t payloadQueueStack[PAYLOAD_MANAGER_QUEUE_LENGTH * PAYLOAD_MANAGER_QUEUE_ITEM_SIZE];

void initPayloadManager(void) {
  ASSERT((payloadQueueStack != NULL) && (&payloadQueue != NULL));
  if (payloadQueueHandle == NULL) {
    payloadQueueHandle = xQueueCreateStatic(PAYLOAD_MANAGER_QUEUE_LENGTH, PAYLOAD_MANAGER_QUEUE_ITEM_SIZE,
                                            payloadQueueStack, &payloadQueue);
  }
}

obc_error_code_t sendToPayloadQueue(payload_event_t *event) {
  ASSERT(payloadQueueHandle != NULL);

  if (event == NULL) return OBC_ERR_CODE_INVALID_ARG;

  if (xQueueSend(payloadQueueHandle, (void *)event, PAYLOAD_MANAGER_QUEUE_TX_WAIT_PERIOD) == pdPASS)
    return OBC_ERR_CODE_SUCCESS;

  return OBC_ERR_CODE_QUEUE_FULL;
}

void obcTaskFunctionPayloadMgr(void *pvParameters) {
  ASSERT(payloadQueueHandle != NULL);

  while (1) {
    payload_event_t queueMsg;
    if (xQueueReceive(payloadQueueHandle, &queueMsg, PAYLOAD_MANAGER_QUEUE_RX_WAIT_PERIOD) != pdPASS)
      queueMsg.eventID = PAYLOAD_MANAGER_NULL_EVENT_ID;

    switch (queueMsg.eventID) {
      case PAYLOAD_MANAGER_NULL_EVENT_ID:
        break;
    }
  }
}