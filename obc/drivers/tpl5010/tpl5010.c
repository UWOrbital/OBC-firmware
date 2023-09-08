#include "tpl5010.h"
#include "gio.h"

#include "obc_i2c_io.h"
#include "obc_logging.h"
#include "obc_errors.h"
#include "obc_assert.h"
#include "obc_board_config.h"

#define TPL5010_WAKE_PIN 0
#define TPL5010_DONE_PIN 1
#define TPL5010_GIO_PORT gioPORTB

#define DONE_SIGNAL_ON 1
#define DONE_SIGNAL_OFF 0

void tpl5010Init(void) { gioEnableNotification(TPL5010_GIO_PORT, TPL5010_WAKE_PIN); }

void feedHardwareWatchdog(void) {
  gioSetPort(TPL5010_GIO_PORT, DONE_SIGNAL_ON);  // pull high then low

  gioSetPort(TPL5010_GIO_PORT, DONE_SIGNAL_OFF);
}
