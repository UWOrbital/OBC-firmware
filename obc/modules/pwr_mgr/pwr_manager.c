#include "pwr_manager.h"
#include "obc_scheduler_config.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_queue.h>
#include <os_task.h>

#include <sys_common.h>
#include <gio.h>

/* PWR queue config */
#define PWR_MANAGER_QUEUE_LENGTH 10U
#define PWR_MANAGER_QUEUE_ITEM_SIZE sizeof(pwr_event_t)
#define PWR_MANAGER_QUEUE_RX_WAIT_PERIOD pdMS_TO_TICKS(10)
#define PWR_MANAGER_QUEUE_TX_WAIT_PERIOD pdMS_TO_TICKS(10)

static QueueHandle_t pwrQueueHandle = NULL;
static StaticQueue_t pwrQueue;
static uint8_t pwrQueueStack[PWR_MANAGER_QUEUE_LENGTH * PWR_MANAGER_QUEUE_ITEM_SIZE];

void obcTaskInitPwrMgr(void) {
  ASSERT((pwrQueueStack != NULL) && (&pwrQueue != NULL));
  if (pwrQueueHandle == NULL) {
    pwrQueueHandle = xQueueCreateStatic(
        PWR_MANAGER_QUEUE_LENGTH, PWR_MANAGER_QUEUE_ITEM_SIZE, pwrQueueStack, &pwrQueue);
  }
}

obc_error_code_t sendToPWRQueue(pwr_event_t *event) {
  ASSERT(pwrQueueHandle != NULL);

  if (event == NULL) return OBC_ERR_CODE_INVALID_ARG;

  if (xQueueSend(pwrQueueHandle, (void *)event, PWR_MANAGER_QUEUE_TX_WAIT_PERIOD) == pdPASS)
    return OBC_ERR_CODE_SUCCESS;

  return OBC_ERR_CODE_QUEUE_FULL;
}

void obcTaskFunctionPwrMgr(void *pvParameters) {
  ASSERT(pwrQueueHandle != NULL);

  while (1) {
    pwr_event_t queueMsg;
    queueMsg.eventID = PWR_MANAGER_EVENT_NONE;
    if (xQueueReceive(pwrQueueHandle, &queueMsg, PWR_MANAGER_QUEUE_RX_WAIT_PERIOD) != pdPASS) {
      switch (queueMsg.eventID) {
        case PWR_MANAGER_OVERCURRENT_DETECTED:
          break;
        case PWR_MANAGER_EVENT_NONE:
          break;
      }
    }
  }
}

void overcurrentInterruptCallback(void) {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  pwr_event_t event = {.eventID = PWR_MANAGER_OVERCURRENT_DETECTED};
  xQueueSendToFrontFromISR(pwrQueueHandle, (void *)&event, &xHigherPriorityTaskWoken);

  if (xHigherPriorityTaskWoken) {
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }
}
