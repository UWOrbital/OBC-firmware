#include "obc_logging.h"
#include "supervisor.h"
#include "obc_sci_io.h"
#include "obc_i2c_io.h"
#include "obc_spi_io.h"

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

int main(void) {
  // Run hardware initialization code
  gioInit();
  sciInit();

  char str[] = "Hello, OBC\r\n";
  sciSend(scilinREG, sizeof(str), (uint8 *)str);

  while (1) {
    gioToggleBit(gioPORTB, 1);
    for (volatile int i = 0; i < 10000000; i++) {
      asm(" NOP");
    }
  }

  // sciInit();
  // i2cInit();
  // spiInit();
  // canInit();
  // hetInit();

  // _enable_interrupt_();

  // // Initialize logger
  // initLogger();

  // // Initialize bus mutexes
  // initSciMutex();
  // initI2CMutex();
  // initSpiMutex();

  // // The supervisor is the only task running initially.
  // initSupervisor();

  // vTaskStartScheduler();
}
