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
#include "obc_persistent.h"

#include <stdbool.h>

void log_result(obc_error_code_t retErrorCode, const char *peripheral, const char *protocol) {
  char *strBuf[100];
  if (retErrorCode != OBC_ERR_CODE_SUCCESS) {
    strcat(strBuf, "POWER ON TEST FAIL: Bad connection with ");
    strcat(strBuf, peripheral);
    strcat(strBuf, " (via ");
    strcat(strBuf, protocol);
    strcat(strBuf, ")\r\n");

    LOG_ERROR_CODE(retErrorCode);
    sciPrintf(strBuf);
  } else {
    strcat(strBuf, "Good connection with ");
    strcat(strBuf, peripheral);
    strcat(strBuf, " (via ");
    strcat(strBuf, protocol);
    strcat(strBuf, ")\r\n");
    sciPrintf("Good connection with LM75BD (via I2C)\r\n");
  }
}

void run_test() {
  obc_error_code_t errCode;
  bool pass = true;

  float placeholder_float;
  uint8_t placeholder_byte;
  uint32_t placeholder_uint32;

  // Test connection with LM75BD
  errCode = readThystLM75BD(LM75BD_OBC_I2C_ADDR, &placeholder_float);
  pass &= (errCode != OBC_ERR_CODE_SUCCESS);
  log_result(errCode, "LM75BD", "I2C");

  // Test connection with CC1120
  errCode = cc1120StrobeSpi(CC1120_STROBE_SRES);
  pass &= (errCode != OBC_ERR_CODE_SUCCESS);
  log_result(errCode, "CC1120", "SPI");

  // Test connection with fram
  testing_persist_data_t data = {.testData = 0x1818};
  errCode = setPersistentData(OBC_PERSIST_SECTION_ID_TESTING, &data, sizeof(testing_persist_data_t));
  pass &= (errCode != OBC_ERR_CODE_SUCCESS);
  log_result(errCode, "FRAM (writing)", "SPI");

  testing_persist_data_t readData;
  errCode = getPersistentData(OBC_PERSIST_SECTION_ID_TESTING, &readData, sizeof(testing_persist_data_t));
  pass &= (errCode != OBC_ERR_CODE_SUCCESS);
  log_result(errCode, "FRAM (reading)", "SPI");

  if (readData.testData != data.testData) {
    errCode = OBC_ERR_CODE_PERSISTENT_CORRUPTED;
    pass = false;
    log_result(errCode, "FRAM (comparing)", "SPI");
  }

  // Test connection with rffm6404
  errCode = rffm6404ActivateRx();
  pass &= (errCode != OBC_ERR_CODE_SUCCESS);
  log_result(errCode, "RFFM6404", "GIO");
  rffm6404PowerOff();  // Return to starting condition

  // Test connection with arducam - part 1
  errCode = initCam();  // Init does a lot of writing to registers over I2C
  pass &= (errCode != OBC_ERR_CODE_SUCCESS);
  log_result(errCode, "Arducam (Init)", "I2C");

  // Test connection with arducam - part 2
  // TODO: Read from SPI test reg when driver is finished

  // errCode = arducamReadTestReg(&placeholder_byte);  // Now testing SPI interface
  // pass &= (errCode != OBC_ERR_CODE_SUCCESS);
  // log_result(errCode, "Arducam", "SPI");

  // Test connection with DS3232
  errCode = getSecondsRTC(&placeholder_byte);
  pass &= (errCode != OBC_ERR_CODE_SUCCESS);
  log_result(errCode, "DS3232", "I2C");

  // Test connection with VN100
  errCode = vn100ReadBaudrate(&placeholder_uint32);  // Handles check that response is a valid baudrate
  pass &= (errCode != OBC_ERR_CODE_SUCCESS);
  log_result(errCode, "VN100", "SCI");

  if (pass) {
    sciPrintf("POWER ON TEST COMPLETE: PASS\r\n");
  } else {
    sciPrintf("POWER ON TEST COMPLETE: FAIL - SEE PREVIOUS OUTPUTS\r\n");
  }
}
