#include <stdio.h>
#include <string.h>
#include "obc_print.h"
#include "obc_logging.h"
#include "obc_sci_io.h"
#include "obc_i2c_io.h"
#include "obc_spi_io.h"
#include "obc_reset.h"
#include "obc_scheduler_config.h"
#include "state_mgr.h"
#include "cc1120.h"

#include <FreeRTOS.h>
#include <os_task.h>

#include <sys_common.h>
#include <sys_core.h>
#include <gio.h>
#include <sci.h>
#include <i2c.h>
#include <spi.h>
#include <can.h>
#include <het.h>

#include "cc1120_defs.h"
#include "cc1120_mcu.h"

// This is the stack canary. It should never be overwritten.
// We are using the CC1120 to generate a random value for the stack canary.
uint8_t __stack_chk_guard = 0;
uint8_t __stack_chk_guard_init(void);

static void __attribute__((constructor, no_stack_protector)) __construct_stk_chk_guard() {
  if (__stack_chk_guard == 0) {
    __stack_chk_guard = __stack_chk_guard_init();
  }
}

uint8_t __stack_chk_guard_init(void) {
  uint8_t received_signal;
  uint8_t stack_canary;
  cc1120Rng(&stack_canary, &received_signal);
  return stack_canary;
}

void __stack_chk_fail(void) { resetSystem(RESET_REASON_STACK_CHECK_FAIL); }

/// @brief stack canary function check
// There should be a reset when stack overflow occurs

static StackType_t stack[1024];
static StaticTask_t taskBuf;

void vTask(void *pvParameters);
void vTask(void *pvParameters) {
  cc1120Init();

  for (int i = 1; i < 3; i++) {
    obc_error_code_t errCode;

    uint8_t canaryValue = 0;
    uint8_t receiver = 0;
    LOG_IF_ERROR_CODE(cc1120Rng(&canaryValue, &receiver));
    if (errCode != OBC_ERR_CODE_SUCCESS) {
      sciPrintf("can't read from ext address \r\n");
    } else {
      sciPrintf("random value: %x , fifo: %x\r\n", canaryValue, receiver);
    }
  }
  vTaskDelay(pdMS_TO_TICKS(1000));
  vTaskDelete(NULL);
}

int main(void) {
  // Run hardware initialization code
  gioInit();
  sciInit();
  i2cInit();
  spiInit();
  canInit();
  hetInit();
  cc1120Init();

  _enable_interrupt_();

  // Initialize bus mutexes
  initSciMutex();
  initI2CMutex();
  initSpiMutex();

  // The state_mgr is the only task running initially.
  obcSchedulerInitTask(OBC_SCHEDULER_CONFIG_ID_STATE_MGR);
  obcSchedulerCreateTask(OBC_SCHEDULER_CONFIG_ID_STATE_MGR);

  sciPrintf("Testing stack canary \n");

  xTaskCreateStatic(vTask, "stack canary test", 1024, NULL, 1, stack, &taskBuf);

  vTaskStartScheduler();
}
