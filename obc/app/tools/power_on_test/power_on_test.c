#include "obc_logging.h"
#include "obc_print.h"
#include "obc_errors.h"
#include "lm75bd.h"
#include "cc1120.h"
#include "cc1120_defs.h"

#include <stdbool.h>

void run_test() {
  obc_error_code_t errCode;
  bool pass = true;

  float placeholder;

  // Test connection with LM75BD
  errCode = readThystLM75BD(LM75BD_OBC_I2C_ADDR, &placeholder);
  if (errCode != OBC_ERR_CODE_SUCCESS) {
    LOG_ERROR_CODE(errCode);
    sciPrintf("POWER ON TEST FAIL: Bad connection with LM75BD (via I2C)");
    pass = false;
  } else {
    sciPrintf("Good connection with LM75BD (via I2C)");
  }

  // Test connection with CC1120
  errCode = cc1120StrobeSpi(CC1120_STROBE_SRES);
  if (errCode != OBC_ERR_CODE_SUCCESS) {
    LOG_ERROR_CODE(errCode);
    sciPrintf("POWER ON TEST FAIL: Bad connection with CC1120 (via SPI)");
    pass = false;
  } else {
    sciPrintf("Good connection with CC1120 (via SPI)");
  }

  // Continue for many peripherals

  if (pass) {
    sciPrintf("POWER ON TEST COMPLETE: PASS");
  } else {
    sciPrintf("POWER ON TEST COMPLETE: FAIL");
  }
}
