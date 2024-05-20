#include "obc_logging.h"
#include "obc_errors.h"
#include "obc_print.h"
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
  uint32_t placeholder_uint32;

  // Test connection with LM75BD
  errCode = readThystLM75BD(LM75BD_OBC_I2C_ADDR, &placeholder_float);
  pass &= (errCode != OBC_ERR_CODE_SUCCESS);
  if (errCode != OBC_ERR_CODE_SUCCESS) {
    LOG_ERROR_CODE(errCode);
    sciPrintf("POWER ON TEST FAIL: Bad connection with LM75BD (via I2C)\r\n");
  } else {
    sciPrintf("Good connection with LM75BD (via I2C)\r\n");
  }

  // Test connection with CC1120
  errCode = cc1120StrobeSpi(CC1120_STROBE_SRES);
  pass &= (errCode != OBC_ERR_CODE_SUCCESS);
  if (errCode != OBC_ERR_CODE_SUCCESS) {
    LOG_ERROR_CODE(errCode);
    sciPrintf("POWER ON TEST FAIL: Bad connection with CC1120 (via SPI)\r\n");
  } else {
    sciPrintf("Good connection with CC1120 (via SPI)\r\n");
  }

  // Test connection with fram
  errCode = framReadID(&placeholder_byte, 1);
  pass &= (errCode != OBC_ERR_CODE_SUCCESS);
  if (errCode != OBC_ERR_CODE_SUCCESS) {
    LOG_ERROR_CODE(errCode);
    sciPrintf("POWER ON TEST FAIL: Bad connection with fram (via SPI)\r\n");
  } else {
    sciPrintf("Good connection with fram (via SPI)\r\n");
  }

  // Test connection with rffm6404
  errCode = rffm6404ActivateRx();
  pass &= (errCode != OBC_ERR_CODE_SUCCESS);
  if (errCode != OBC_ERR_CODE_SUCCESS) {
    LOG_ERROR_CODE(errCode);
    sciPrintf("POWER ON TEST FAIL: Bad connection with rffm6404 (via GIO)\r\n");
  } else {
    sciPrintf("Good connection with rffm6404 (via GIO)\r\n");
  }
  rffm6404PowerOff();  // Return to starting condition

  // Test connection with arducam - part 1
  errCode = initCam();  // Init does a lot of writing to registers over both I2C
  pass &= (errCode != OBC_ERR_CODE_SUCCESS);
  if (errCode != OBC_ERR_CODE_SUCCESS) {
    LOG_ERROR_CODE(errCode);
    sciPrintf("POWER ON TEST FAIL: Bad connection with Arducam (I2C)\r\n");
  } else {
    sciPrintf("Good connection with Arducam (I2C)\r\n");
  }

  // Test connection with arducam - part 2
  // TODO: Read from SPI test reg when driver is finished

  // errCode = arducamReadTestReg(&placeholder_byte);  // Now testing SPI interface
  // pass &= (errCode != OBC_ERR_CODE_SUCCESS);
  // if (errCode != OBC_ERR_CODE_SUCCESS) {
  //   LOG_ERROR_CODE(errCode);
  //   sciPrintf("POWER ON TEST FAIL: Bad connection with Arducam (SPI)\r\n");
  // } else {
  //   sciPrintf("Good connection with Arducam (SPI)\r\n");
  // }

  // Test connection with DS3232
  errCode = getSecondsRTC(&placeholder_byte);
  pass &= (errCode != OBC_ERR_CODE_SUCCESS);
  if (errCode != OBC_ERR_CODE_SUCCESS) {
    LOG_ERROR_CODE(errCode);
    sciPrintf("POWER ON TEST FAIL: Bad connection with DS3232 (via I2C)\r\n");
  } else {
    sciPrintf("Good connection with DS3232 (via I2C)\r\n");
  }

  // Test connection with vn100
  errCode = vn100ReadBaudrate(&placeholder_uint32);  // Handles check that response is a valid baudrate
  pass &= (errCode != OBC_ERR_CODE_SUCCESS);
  if (errCode != OBC_ERR_CODE_SUCCESS) {
    LOG_ERROR_CODE(errCode);
    sciPrintf("POWER ON TEST FAIL: Bad connection with VN100 (via SCI)\r\n");
  } else {
    sciPrintf("Good connection with VN100 (via SCI)\r\n");
  }

  if (pass) {
    sciPrintf("POWER ON TEST COMPLETE: PASS\r\n");
  } else {
    sciPrintf("POWER ON TEST COMPLETE: FAIL - SEE PREVIOUS OUTPUTS\r\n");
  }
}
