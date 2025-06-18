#include "gnc_manager.h"
#include "obc_errors.h"
#include "obc_scheduler_config.h"
#include "digital_watchdog_mgr.h"

#include "obc_logging.h"  //include logging for testing
#include <stdio.h>        //include for testing

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_task.h>

#include <sys_common.h>
#include <gio.h>

#define GNC_TASK_PERIOD_MS 50 /* 50ms period or 20Hz */

void obcTaskInitGncMgr(void) {}

void obcTaskFunctionGncMgr(void *pvParameters) {
  TickType_t xLastWakeTime;

  /* Initialize the last wake time to the current time */
  xLastWakeTime = xTaskGetTickCount();

  // Spam logs at startup
  for (int i = 0; i < 20; i++) {
    // LOG_DEBUG("BLOCK TEST LOG");
    LOG_ERROR_CODE(i);
    // printf("Finished log %d\n", i);
  }

  /* Run GNC tasks periodically at 20 Hz */
  while (1) {
    digitalWatchdogTaskCheckIn(OBC_SCHEDULER_CONFIG_ID_GNC_MGR);

    /* This will automatically update the xLastWakeTime variable to be the last unblocked time */
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(GNC_TASK_PERIOD_MS));
  }
}
