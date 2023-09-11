#include "obc_logging.h"
#include "supervisor.h"
#include "obc_sci_io.h"
#include "obc_i2c_io.h"
#include "obc_spi_io.h"
#include "obc_reset.h"

#include <FreeRTOS.h>
#include <os_task.h>
#include "obc_board_config.h"

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
  gioSetBit(gioPORTA, 1, 1);
  gioSetBit(gioPORTA, 0, 1);
  _enable_interrupt_();
  gioDisableNotification(gioPORTB, CC1120_RX_THR_PKT_gioPORTB_PIN);
  gioDisableNotification(gioPORTA, CC1120_TX_THR_PKT_gioPORTA_PIN);
  gioDisableNotification(gioPORTA, CC1120_PKT_SYNC_RXTX_gioPORTA_PIN);

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
