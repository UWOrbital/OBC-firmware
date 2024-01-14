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

void obcTaskInitGncMgr(void) {}

void obcTaskFunctionGncMgr(void *pvParameters) {
  TickType_t xLastWakeTime;

  // Initialize the last wake time to the current time
  xLastWakeTime = xTaskGetTickCount();

  while (1) {
    feedDigitalWatchdog();

    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(GNC_TASK_PERIOD_MS));
  }
}
