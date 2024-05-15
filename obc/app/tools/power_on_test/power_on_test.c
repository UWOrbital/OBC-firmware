#include "obc_logging.h"
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
  pass &= (errCode != OBC_ERR_CODE_SUCCESS);
  if (errCode != OBC_ERR_CODE_SUCCESS) {
    LOG_ERROR_CODE(errCode);
    LOG_DEBUG("POWER ON TEST FAIL: Bad connection with LM75BD (via I2C)");
  } else {
    LOG_DEBUG("Good connection with LM75BD (via I2C)");
  }

  // Test connection with CC1120
  errCode = cc1120StrobeSpi(CC1120_STROBE_SRES);
  pass &= (errCode != OBC_ERR_CODE_SUCCESS);
  if (errCode != OBC_ERR_CODE_SUCCESS) {
    LOG_ERROR_CODE(errCode);
    LOG_DEBUG("POWER ON TEST FAIL: Bad connection with CC1120 (via SPI)");
  } else {
    LOG_DEBUG("Good connection with CC1120 (via SPI)");
  }

  // Test connection with fram
  errCode = framReadID(&placeholder_byte, 1);
  pass &= (errCode != OBC_ERR_CODE_SUCCESS);
  if (errCode != OBC_ERR_CODE_SUCCESS) {
    LOG_ERROR_CODE(errCode);
    LOG_DEBUG("POWER ON TEST FAIL: Bad connection with fram (via SPI)");
  } else {
    LOG_DEBUG("Good connection with fram (via SPI)");
  }

  // Test connection with rffm6404
  errCode = rffm6404ActivateRx();
  pass &= (errCode != OBC_ERR_CODE_SUCCESS);
  if (errCode != OBC_ERR_CODE_SUCCESS) {
    LOG_ERROR_CODE(errCode);
    LOG_DEBUG("POWER ON TEST FAIL: Bad connection with rffm6404 (via GIO)");
  } else {
    LOG_DEBUG("Good connection with rffm6404 (via GIO)");
  }
  rffm6404PowerOff();  // Return to starting condition

  // Test connection with arducam - part 1
  errCode = initCam();  // Init does a lot of writing to registers over both I2C
  pass &= (errCode != OBC_ERR_CODE_SUCCESS);
  if (errCode != OBC_ERR_CODE_SUCCESS) {
    LOG_ERROR_CODE(errCode);
    LOG_DEBUG("POWER ON TEST FAIL: Bad connection with Arducam (I2C)");
  } else {
    LOG_DEBUG("Good connection with Arducam (I2C)");
  }

  // Test connection with arducam - part 2
  errCode = flushFifo(PRIMARY);  // Now testing SPI interface
  pass &= (errCode != OBC_ERR_CODE_SUCCESS);
  if (errCode != OBC_ERR_CODE_SUCCESS) {
    LOG_ERROR_CODE(errCode);
    LOG_DEBUG("POWER ON TEST FAIL: Bad connection with Arducam (SPI)");
  } else {
    LOG_DEBUG("Good connection with Arducam (SPI)");
  }

  // Test connection with DS3232
  errCode = getSecondsRTC(&placeholder_byte);
  pass &= (errCode != OBC_ERR_CODE_SUCCESS);
  if (errCode != OBC_ERR_CODE_SUCCESS) {
    LOG_ERROR_CODE(errCode);
    LOG_DEBUG("POWER ON TEST FAIL: Bad connection with DS3232 (via I2C)");
  } else {
    LOG_DEBUG("Good connection with DS3232 (via I2C)");
  }

  // Test connection with vn100
  errCode = vn100SetBaudrate(115200);
  pass &= (errCode != OBC_ERR_CODE_SUCCESS);
  if (errCode != OBC_ERR_CODE_SUCCESS) {
    LOG_ERROR_CODE(errCode);
    LOG_DEBUG("POWER ON TEST FAIL: Bad connection with VN100 (via SCI)");
  } else {
    LOG_DEBUG("Good connection with VN100 (via SCI)");
  }

  if (pass) {
    LOG_DEBUG("POWER ON TEST COMPLETE: PASS");
  } else {
    LOG_DEBUG("POWER ON TEST COMPLETE: FAIL - SEE PREVIOUS OUTPUTS");
  }
}
