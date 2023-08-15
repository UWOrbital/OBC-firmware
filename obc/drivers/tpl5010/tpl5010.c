#include "tpl5010.h"
#include "gio.h"

#include "obc_i2c_io.h"
#include "obc_logging.h"
#include "obc_errors.h"
#include "obc_assert.h"

#define DONE_SIGNAL_ON 1
#define DONE_SIGNAL_OFF 0

obc_error_code_t tpl5010Init(void) {
  gioEnableNotification(gioPORTB, TPL5010_WAKE_PIN);

  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t tpl5010Reset(void) {
  gioSetPort(gioPORTB, DONE_SIGNAL_ON);  // pull high then low

  gioSetPort(gioPORTB, DONE_SIGNAL_OFF);

  return OBC_ERR_CODE_SUCCESS;
}
