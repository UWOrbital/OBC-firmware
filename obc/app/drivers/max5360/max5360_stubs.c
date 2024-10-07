#ifndef CONFIG_MAX5360
#include "obc_errors.h"
#include "obc_general_util.h"
#include "max5360.h"

obc_error_code_t max5360WriteVoltage(float analogVoltsOutput) {
  UNUSED(analogVoltsOutput);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t max5360PowerOff(void) { return OBC_ERR_CODE_SUCCESS; }

#endif  // CONFIG_MAX5360
