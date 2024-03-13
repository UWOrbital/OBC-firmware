#include "comms_manager.h"
#include "downlink_encoder.h"
#include "obc_errors.h"
#include "obc_gs_telemetry_data.h"
#include "obc_logging.h"
#include "obc_sci_io.h"
#include "obc_i2c_io.h"
#include "obc_spi_io.h"
#include "obc_reset.h"
#include "obc_scheduler_config.h"
#include "state_mgr.h"

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

// Rm:
#include <string.h>
#include "obc_print.h"
#include "cc1120_txrx.h"
#include "cc1120.h"
#include "obc_gs_aes128.h"
#include "obc_gs_fec.h"

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

  // Initialize bus mutexes
  initSciMutex();
  initI2CMutex();
  initSpiMutex();

  /*
  obc_error_code_t errCode;
  static const uint8_t TEMP_STATIC_KEY[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                            0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
  initializeAesCtx(TEMP_STATIC_KEY);
  initRs();
  initAllCc1120TxRxSemaphores();
  LOG_IF_ERROR_CODE(cc1120Init());
  uint8_t buff[300];
  memset(buff, 0xA5, 300);
  while (1) {
    cc1120Send(buff, 300, 500);
  }
*/
  static comms_state_t commsManagerState = COMMS_STATE_DISCONNECTED;
  obcSchedulerInitTask(OBC_SCHEDULER_CONFIG_ID_COMMS_MGR);
  obcSchedulerInitTask(OBC_SCHEDULER_CONFIG_ID_COMMS_DOWNLINK_ENCODER);
  obcSchedulerCreateTaskWithArgs(OBC_SCHEDULER_CONFIG_ID_COMMS_MGR, &commsManagerState);
  obcSchedulerInitTask(OBC_SCHEDULER_CONFIG_ID_COMMS_DOWNLINK_ENCODER);

  // The state_mgr is the only task running initially.
  // obcSchedulerInitTask(OBC_SCHEDULER_CONFIG_ID_STATE_MGR);
  // obcSchedulerCreateTask(OBC_SCHEDULER_CONFIG_ID_STATE_MGR);
  vTaskStartScheduler();
}
