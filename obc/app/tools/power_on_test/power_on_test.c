#include "obc_logging.h"
#include "obc_print.h"
#include "obc_errors.h"
#include "lm75bd.h"
#include "cc1120.h"
#include "cc1120_defs.h"
#include "fm25v20a.h"
#include "rffm6404.h"
#include "arducam.h"
#include "ds3232_mz.h"
#include "vn100.h"

#include <stdbool.h>

void run_test() {
  obc_error_code_t errCode;
  bool pass = true;

  float placeholder_float;
  uint8_t placeholder_byte;

  // Test connection with LM75BD
  errCode = readThystLM75BD(LM75BD_OBC_I2C_ADDR, &placeholder_float);
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

  // Test connection with fram
  errCode = framWakeUp();
  if (errCode != OBC_ERR_CODE_SUCCESS) {
    LOG_ERROR_CODE(errCode);
    sciPrintf("POWER ON TEST FAIL: Bad connection with fram (via SPI)");
    pass = false;
  } else {
    sciPrintf("Good connection with fram (via SPI)");
  }

  // Test connection with rffm6404
  errCode = rffm6404ActivateRx();
  if (errCode != OBC_ERR_CODE_SUCCESS) {
    LOG_ERROR_CODE(errCode);
    sciPrintf("POWER ON TEST FAIL: Bad connection with rffm6404 (via GIO)");
    pass = false;
  } else {
    sciPrintf("Good connection with rffm6404 (via GIO)");
  }

  // Test connection with arducam
  errCode = initCam();
  if (errCode != OBC_ERR_CODE_SUCCESS) {
    LOG_ERROR_CODE(errCode);
    sciPrintf("POWER ON TEST FAIL: Bad connection with Arducam (via SPI)");
    pass = false;
  } else {
    sciPrintf("Good connection with Arducam (via SPI)");
  }

  // Test connection with DS3232
  errCode = getSecondsRTC(&placeholder_byte);
  if (errCode != OBC_ERR_CODE_SUCCESS) {
    LOG_ERROR_CODE(errCode);
    sciPrintf("POWER ON TEST FAIL: Bad connection with DS3232 (via I2C)");
    pass = false;
  } else {
    sciPrintf("Good connection with DS3232 (via I2C)");
  }

  // Test connection with vn100
  errCode = vn100SetBaudrate(115200);
  if (errCode != OBC_ERR_CODE_SUCCESS) {
    LOG_ERROR_CODE(errCode);
    sciPrintf("POWER ON TEST FAIL: Bad connection with VN100 (via SCI)");
    pass = false;
  } else {
    sciPrintf("Good connection with VN100 (via SCI)");
  }

  if (pass) {
    sciPrintf("POWER ON TEST COMPLETE: PASS");
  } else {
    sciPrintf("POWER ON TEST COMPLETE: FAIL");
  }
}
