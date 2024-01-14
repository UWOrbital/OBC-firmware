#include "gnc_manager.h"
#include "obc_digital_watchdog.h"
#include "obc_errors.h"
#include "obc_scheduler_config.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_queue.h>
#include <os_task.h>

#include <sys_common.h>
#include <gio.h>

#define GNC_TASK_PERIOD_MS 50 /* 50ms period or 20Hz */

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

  TickType_t xLastWakeTime;

  // Initialize the last wake time to the current time
  xLastWakeTime = xTaskGetTickCount();

  while (1) {
    feedDigitalWatchdog();

    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(GNC_TASK_PERIOD_MS));
  }
}
