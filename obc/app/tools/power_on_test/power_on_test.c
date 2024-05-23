#include "obc_logging.h"
#include "obc_errors.h"
#include "obc_print.h"
#include "obc_persistent.h"
#include "obc_reliance_fs.h"
#include "telemetry_fs_utils.h"
#include "lm75bd.h"
#include "cc1120.h"
#include "cc1120_defs.h"
#include "rffm6404.h"
#include "arducam.h"
#include "ds3232_mz.h"
#include "vn100.h"

#include <redposix.h>

#include <stdbool.h>
#include <stdio.h>

void log_result(obc_error_code_t retErrorCode, const char *peripheral, const char *protocol) {
  char strBuf[60 + strlen(peripheral) +
              strlen(protocol)];  // Approx 50 for fail text plus some wiggle room if message changes
  if (retErrorCode != OBC_ERR_CODE_SUCCESS) {
    snprintf(strBuf, sizeof(strBuf), "POWER ON TEST FAIL: Bad connection with %s (via %s)\r\n", peripheral, protocol);
    LOG_ERROR_CODE(retErrorCode);
  } else {
    snprintf(strBuf, sizeof(strBuf), "Good connection with %s (via %s)\r\n", peripheral, protocol);
  }

  sciPrintf(strBuf);
}

void run_test() {
  obc_error_code_t errCode;
  bool pass = true;

  float placeholder_float;
  uint8_t placeholder_byte;
  uint32_t placeholder_uint32;
  size_t placeholder_size;

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
  }
  log_result(errCode, "FRAM (comparing)", "SPI");

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

  // Test connection with SD Card
  int32_t fileId;
  const char writeData[] = "UW Orbital";
  const char filePath[] = "/power_on_test.txt";
  char readBuf[strlen(writeData)];

  setupFileSystem();
  errCode = createFile(filePath, &fileId);
  red_open(filePath, RED_O_RDWR | RED_O_APPEND);

  if (errCode != OBC_ERR_CODE_SUCCESS) {
    pass = false;
    log_result(errCode, "SD Card (create & open file)", "SPI");
  } else {
    errCode = writeFile(fileId, writeData, strlen(writeData));
    if (errCode != OBC_ERR_CODE_SUCCESS) {
      pass = false;
      log_result(errCode, "SD Card (write)", "SPI");
    } else {
      readFile(fileId, readBuf, strlen(writeData), &placeholder_size);

      if (strcmp(writeData, readBuf) != 0) {
        errCode = OBC_ERR_CODE_UNKNOWN;
        pass = false;
        log_result(errCode, "SD Card (compare)", "SPI");
      }

      deleteFile(filePath);
    }
  }
  log_result(errCode, "SD Card (overall)", "SPI");

  if (pass) {
    sciPrintf("POWER ON TEST COMPLETE: PASS\r\n");
  } else {
    sciPrintf("POWER ON TEST COMPLETE: FAIL - SEE PREVIOUS OUTPUTS\r\n");
  }
}
