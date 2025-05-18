#include "timekeeper.h"
#include "obc_time.h"
#include "obc_errors.h"
#include "obc_assert.h"
#include "obc_logging.h"
#include "obc_persistent.h"
#include "obc_scheduler_config.h"
#include "ds3232_mz.h"
#include "obc_reliance_fs.h"

#include <FreeRTOS.h>
#include <os_task.h>
#include <os_timer.h>
#include <sys_common.h>

#include "framSdcIntegTest.h"
#include "obc_print.h"
#include <stdio.h>
#include <redposix.h>

void obcTaskInitSdcFramIntegTest(void *params) { return; }

void obcTaskFunctionSdcFramIntegTest(void *params) {
  obc_error_code_t errCode;
  char printBuff[256];
  obc_time_persist_data_t writeTestTime = {.unixTime = 69420};
  obc_time_persist_data_t readTime = {0};

  const char sdcTestDir[] = "/sdcTestDir/";
  const char sdcTestFilePath[] = "/sdcTestDir/testfile.txt";
  const char sdcTestWriteStr[] = "---\n\rThis is a test string that is written to SDC!\n\r---";
  char sdcReadBuff[strlen(sdcTestWriteStr) + 1];
  int32_t fd = 0;
  size_t bytesRead = 0;

  sciPrintText((unsigned char *)"############################################ Running FRAM + SDC integration test!\n\r",
               strlen("############################################ Running FRAM + SDC integration test!\n\r"), 0xFFFF);

  /* Write test value to FRAM */
  sprintf(printBuff, "======     Testing FRAM read/write   ======\n\r");
  sciPrintText((unsigned char *)printBuff, strlen(printBuff), 0xFFFF);
  LOG_IF_ERROR_CODE(
      setPersistentData(OBC_PERSIST_SECTION_ID_OBC_TIME, (void *)&writeTestTime, sizeof(obc_time_persist_data_t)));

  sprintf(printBuff, "Wrote %ld to FRAM.\n\r", writeTestTime.unixTime);
  sciPrintText((unsigned char *)printBuff, strlen(printBuff), 0xFFFF);

  /* Read test value from FRAM */
  LOG_IF_ERROR_CODE(
      getPersistentDataByIndex(OBC_PERSIST_SECTION_ID_OBC_TIME, 0, (void *)&readTime, sizeof(obc_time_persist_data_t)));

  sprintf(printBuff, "Read %ld from FRAM.\n\r", readTime.unixTime);
  sciPrintText((unsigned char *)printBuff, strlen(printBuff), 0xFFFF);

  if (readTime.unixTime == writeTestTime.unixTime)
    sprintf(printBuff, "PASSED FRAM TEST!\n\r");
  else
    sprintf(printBuff, "FAILED FRAM TEST :(\n\r");
  sciPrintText((unsigned char *)printBuff, strlen(printBuff), 0xFFFF);

  sprintf(printBuff, "====== Finished FRAM write/read test ======\n\r");
  sciPrintText((unsigned char *)printBuff, strlen(printBuff), 0xFFFF);
  /* End FRAM Test */

  /* SDC write/read test */
  /* Write test value to SDC */
  sprintf(printBuff, "======     Testing SDC read/write   ======\n\r");
  sciPrintText((unsigned char *)printBuff, strlen(printBuff), 0xFFFF);
  LOG_IF_ERROR_CODE(mkDir(sdcTestDir));
  LOG_IF_ERROR_CODE(createFile(sdcTestFilePath, &fd));
  LOG_IF_ERROR_CODE(openFile(sdcTestFilePath, RED_O_RDWR | RED_O_APPEND, &fd));
  LOG_IF_ERROR_CODE(writeFile(fd, (void *)sdcTestWriteStr, strlen(sdcTestWriteStr) + 1));
  LOG_IF_ERROR_CODE(closeFile(fd));
  sprintf(printBuff, "Wrote follwoing string to %s and closed the file:\n\r%s\n\r", sdcTestFilePath, sdcTestWriteStr);
  sciPrintText((unsigned char *)printBuff, strlen(printBuff), 0xFFFF);

  LOG_IF_ERROR_CODE(openFile(sdcTestFilePath, RED_O_RDWR | RED_O_APPEND, &fd));
  LOG_IF_ERROR_CODE(readFile(fd, (void *)sdcReadBuff, strlen(sdcTestWriteStr) + 1, &bytesRead));
  LOG_IF_ERROR_CODE(closeFile(fd));
  sprintf(printBuff, "Opened %s and read %d bytes, expected %d bytes.\n\r", sdcTestFilePath, bytesRead,
          strlen(sdcTestWriteStr) + 1);
  sciPrintText((unsigned char *)printBuff, strlen(printBuff), 0xFFFF);

  sprintf(printBuff, "Read string:\n\r%s\n\r", sdcReadBuff);
  sciPrintText((unsigned char *)printBuff, strlen(printBuff), 0xFFFF);

  if (!strcmp(sdcReadBuff, sdcTestWriteStr))
    sprintf(printBuff, "PASSED SDC TEST!\n\r");
  else
    sprintf(printBuff, "FAILED SDC TEST :(\n\r");
  sciPrintText((unsigned char *)printBuff, strlen(printBuff), 0xFFFF);

  sprintf(printBuff, "====== Finished SDC write/read test  ======\n\r");
  sciPrintText((unsigned char *)printBuff, strlen(printBuff), 0xFFFF);

  while (1)
    ;
}
