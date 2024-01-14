#include "gnc_manager.h"
#include "obc_errors.h"
#include "obc_scheduler_config.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_queue.h>
#include <os_task.h>

#include <sys_common.h>
#include <gio.h>

static QueueHandle_t gncQueueHandle = NULL;
static StaticQueue_t gncQueue;
static uint8_t gncQueueStack[GNC_MANAGER_QUEUE_LENGTH * GNC_MANAGER_QUEUE_ITEM_SIZE];

void obcTaskInitGncMgr(void) {
  ASSERT((gncQueueStack != NULL) && (&gncQueue != NULL));
  if (gncQueueHandle == NULL) {
    gncQueueHandle =
        xQueueCreateStatic(GNC_MANAGER_QUEUE_LENGTH, GNC_MANAGER_QUEUE_ITEM_SIZE, gncQueueStack, &gncQueue);
  }
}

obc_error_code_t sendToGncQueue(gnc_event_t *event) {
  ASSERT(gncQueueHandle != NULL);

  if (event == NULL) return OBC_ERR_CODE_INVALID_ARG;

  if (xQueueSend(gncQueueHandle, (void *)event, GNC_MANAGER_QUEUE_TX_WAIT_PERIOD) == pdPASS) {
    return OBC_ERR_CODE_SUCCESS;
  }
  return OBC_ERR_CODE_QUEUE_FULL;
}

void obcTaskFunctionGncMgr(void *pvParameters) {
  ASSERT(gncQueueHandle != NULL);

  while (1) {
    gnc_event_t queueMsg;
    if (xQueueReceive(gncQueueHandle, &queueMsg, GNC_MANAGER_QUEUE_RX_WAIT_PERIOD) == pdTRUE) {
      switch (queueMsg.eventID) {
        case GNC_MANAGER_NULL_EVENT_ID:
          break;

        case SECONDARY_GNC_MANAGER_EVENT_ID:
          // ADD SECONDARY GNC COMMAND HANDLER
          break;
      }
    }
  }
}
