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

#define STACK_BYTES 4

void *__stack_chk_guard = (void *)0xDEADBEEF;

void __stack_chk_fail(void) { resetSystem(RESET_REASON_STACK_CHECK_FAIL); }

uint32_t __stack_chk_guard_init(void);

uint32_t __stack_chk_guard_change(void) {
  obc_error_code_t errCode;
  uint32_t newStackGuard = 0;
  for (uint8_t i = 0; i < STACK_BYTES; i++) {
    uint8_t randomByte;
    errCode = cc1120Rng(&randomByte);
    if (errCode == OBC_ERR_CODE_SUCCESS) {
      (newStackGuard) = (newStackGuard << 8) | randomByte;
    } else {
      LOG_ERROR_CODE(errCode);
      errCode = OBC_ERR_CODE_FAILED_STACK_CANARY;
      LOG_ERROR_CODE(errCode);
      return 0xDEADBEEF;
    }
  }
  return newStackGuard;
}

static void __attribute__((no_stack_protector)) __construct_stk_chk_guard() {
  if (__stack_chk_guard == (void *)0xDEADBEEF) {
    __stack_chk_guard = (void *)__stack_chk_guard_change();
  }
}

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
  cc1120Init();
  __construct_stk_chk_guard();

  // The state_mgr is the only task running initially.
  obcSchedulerInitTask(OBC_SCHEDULER_CONFIG_ID_STATE_MGR);
  obcSchedulerCreateTask(OBC_SCHEDULER_CONFIG_ID_STATE_MGR);

  vTaskStartScheduler();
}
