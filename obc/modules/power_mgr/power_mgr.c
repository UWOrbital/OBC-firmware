#include "power_mgr.h"
#include "obc_print.h"
#include "obc_scheduler_config.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_queue.h>
#include <os_task.h>

#include <sys_common.h>
#include <gio.h>

#define MPPT_PERIOD (pdMS_TO_TICKS(500))

obc_error_code_t mppt_step(void);

void obcTaskInitPowerMgr(void) {}

void obcTaskFunctionPowerMgr(void *pvParameters) {
  while (1) {
    mppt_step();
    vTaskDelay(MPPT_PERIOD);
  }
}

obc_error_code_t mppt_step() {
  sciPrintText((unsigned char *)"MPPT Step\r\n", strlen("MPPT Step\r\n"), portMAX_DELAY);
  return OBC_ERR_CODE_SUCCESS;
}
