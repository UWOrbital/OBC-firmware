#include "obc_logging.h"
#include "supervisor.h"
#include "obc_sci_io.h"
#include "obc_i2c_io.h"
#include "obc_spi_io.h"
#include "vn100.h"

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
  initVN100();
  vTaskStartScheduler();

  vn_ypr_packet_t mypacket;
  retrieveYPR(&mypacket);

  sciPrintf("Yaw: %f \r\n", mypacket.yaw);
  sciPrintf("Pitch: %f \r\n", mypacket.pitch);
  sciPrintf("Roll: %f \r\n", mypacket.roll);
}
