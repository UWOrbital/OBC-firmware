#include "obc_logging.h"
#include "supervisor.h"
#include "obc_sci_io.h"
#include "obc_i2c_io.h"
#include "obc_spi_io.h"
#include "obc_reset.h"

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
// Ideally, it would be a random value, but we don't have a good source of entropy
// that we can use.
void *__stack_chk_guard = (void *)0xDEADBEEF;

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

  // Initialize logger
  initLogger();

  // Initialize bus mutexes
  initSciMutex();
  initI2CMutex();
  initSpiMutex();

  // The supervisor is the only task running initially.
  initSupervisor();

  vTaskStartScheduler();
}
