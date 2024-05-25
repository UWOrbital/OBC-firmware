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

// This is the stack canary. It should never be overwritten.
// We are using the CC1120 to generate a random value for the stack canary.
uintptr_t __stack_chk_guard = 0;

static void __attribute__((constructor, no_stack_protector)) __construct_stk_chk_guard() {
  if (__stack_chk_guard == 0) {
    __stack_chk_guard = __stack_chk_guard_init();
  }
}

uintptr_t __stack_chk_guard_init(void) {
  uint8_t received_signal;
  uintptr_t stack_canary;
  cc1120Rng(&stack_canary, &received_signal);
  return stack_canary;
}

void __stack_chk_fail(void) { resetSystem(RESET_REASON_STACK_CHECK_FAIL); }

int main(void) {
  // Run hardware initialization code
  gioInit();
  sciInit();
  i2cInit();
  spiInit();
  canInit();
  hetInit();

  _enable_interrupt_();

  // Initialize bus mutexes
  initSciMutex();
  initI2CMutex();
  initSpiMutex();

  // The state_mgr is the only task running initially.
  obcSchedulerInitTask(OBC_SCHEDULER_CONFIG_ID_STATE_MGR);
  obcSchedulerCreateTask(OBC_SCHEDULER_CONFIG_ID_STATE_MGR);

  vTaskStartScheduler();
}
