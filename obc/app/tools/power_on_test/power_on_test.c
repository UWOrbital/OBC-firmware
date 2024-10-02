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

#define MAX_MSG_SIZE 128U
#define MAX_FNAME_LINENUM_SIZE 128U
#define MAX_LOG_SIZE (MAX_MSG_SIZE + MAX_FNAME_LINENUM_SIZE + 10U)

static void logResult(bool success, const char *peripheral, const char *protocol, bool *pass) {
  char strBuf[60 + strlen(peripheral) +
              strlen(protocol)];  // Approx 50 for fail text plus some wiggle room if message changes
  if (!success) {
    snprintf(strBuf, sizeof(strBuf), "POWER ON TEST FAIL: Bad connection with %s (via %s)\r\n", peripheral, protocol);
    *pass = false;
  } else {
    snprintf(strBuf, sizeof(strBuf), "Good connection with %s (via %s)\r\n", peripheral, protocol);
  }

  sciPrintf(strBuf);
}

void runTest() {
  obc_error_code_t errCode;
  bool pass = true;

  float placeholderFloat;
  uint32_t placeholderUint32;
  size_t placeholderSize;

  // Test connection with LM75BD
  errCode = readTempLM75BD(LM75BD_OBC_I2C_ADDR, &placeholderFloat);
  logResult(errCode == OBC_ERR_CODE_SUCCESS && placeholderFloat != 0.0, "LM75BD", "I2C", &pass);

  // Test connection with CC1120
  errCode = cc1120StrobeSpi(CC1120_STROBE_SRES);
  logResult(errCode == OBC_ERR_CODE_SUCCESS, "CC1120", "SPI", &pass);

  // Test connection with fram
  testing_persist_data_t data = {.testData = 0x1818};
  errCode = setPersistentData(OBC_PERSIST_SECTION_ID_TESTING, &data, sizeof(testing_persist_data_t));

  if (errCode != OBC_ERR_CODE_SUCCESS) {
    logResult(false, "FRAM", "SPI", &pass);
  } else {
    testing_persist_data_t readData;
    errCode = getPersistentData(OBC_PERSIST_SECTION_ID_TESTING, &readData, sizeof(testing_persist_data_t));

    if (errCode != OBC_ERR_CODE_SUCCESS) {
      logResult(false, "FRAM", "SPI", &pass);
    } else {
      if (readData.testData != data.testData) {
        errCode = OBC_ERR_CODE_PERSISTENT_CORRUPTED;
        logResult(false, "FRAM", "SPI", &pass);
      } else {
        logResult(true, "FRAM", "SPI", &pass);
      }
    }
  }

  // Test connection with rffm6404
  errCode = rffm6404ActivateRx();
  logResult(errCode == OBC_ERR_CODE_SUCCESS, "RFFM6404", "GIO", &pass);
  rffm6404PowerOff();  // Return to starting condition

  // Test connection with arducam
  // TODO: Read from test reg when driver is finished
  // errCode = arducamReadTestReg(&placeholder_byte);  // Now testing SPI interface
  // logResult(errCode == OBC_ERR_CODE_SUCCESS, "Arducam", "SPI", &pass); // Maybe some other validation

  // Test connection with DS3232
  rtc_status_t status;
  errCode = getStatusRTC(&status);
  logResult(errCode == OBC_ERR_CODE_SUCCESS && status.EN32KHZ, "DS3232", "I2C",
            &pass);  // 32khz output enabled by default, will be on power up

  // Test connection with VN100
  errCode = vn100ReadBaudrate(&placeholderUint32);  // Handles check that response is a valid baudrate
  logResult(errCode == OBC_ERR_CODE_SUCCESS, "VN100", "SCI", &pass);

  // Test connection with SD Card
  int32_t fileId;
  const char writeData[] = "Orbital\r\n";
  const char filePath[] = "/power_on_test.txt";
  char readBuf[strlen(writeData)];

  setupFileSystem();
  errCode = createFile(filePath, &fileId);

  if (errCode != OBC_ERR_CODE_SUCCESS) {
    logResult(false, "SD Card", "SPI", &pass);
  } else {
    red_open(filePath, RED_O_RDWR | RED_O_APPEND);
    errCode = writeFile(fileId, writeData, strlen(writeData));

    if (errCode != OBC_ERR_CODE_SUCCESS) {
      logResult(false, "SD Card", "SPI", &pass);
    } else {
      readFile(fileId, readBuf, strlen(writeData), &placeholderSize);

      if (strcmp(writeData, readBuf) != 0) {
        errCode = OBC_ERR_CODE_FAILED_FILE_READ;
        logResult(false, "SD Card", "SPI", &pass);
      } else {
        logResult(true, "SD Card", "SPI", &pass);
      }

      deleteFile(filePath);
    }
  }

  if (pass) {
    sciPrintf("POWER ON TEST COMPLETE: PASS\r\n");
  } else {
    sciPrintf("POWER ON TEST COMPLETE: FAIL - SEE PREVIOUS OUTPUTS\r\n");
  }
}
