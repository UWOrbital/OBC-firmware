#include "timekeeper.h"
#include "obc_time.h"
#include "obc_errors.h"
#include "obc_assert.h"
#include "obc_logging.h"
#include "obc_persistent.h"
#include "obc_scheduler_config.h"
#include "ds3232_mz.h"

#include <FreeRTOS.h>
#include <os_task.h>
#include <os_timer.h>
#include <sys_common.h>

#include "framSdcIntegTest.h"
#include "obc_print.h"
#include <stdio.h>

void obcTaskInitSdcFramIntegTest(void *params) { return; }

void obcTaskFunctionSdcFramIntegTest(void *params) {
  obc_error_code_t errCode;
  char printBuff[256];
  obc_time_persist_data_t writeTestTime = {.unixTime = 69420};
  obc_time_persist_data_t readTime = {0};

  sciPrintText((unsigned char *)"############################################ Running FRAM + SDC integration test!\n\r",
               strlen("############################################ Running FRAM + SDC integration test!\n\r"), 0xFFFF);

  /* Write test value to FRAM */
  LOG_IF_ERROR_CODE(
      setPersistentData(OBC_PERSIST_SECTION_ID_OBC_TIME, (void *)&writeTestTime, sizeof(obc_time_persist_data_t)));

  sprintf(printBuff, "Wrote %ld to FRAM.\n\r", writeTestTime.unixTime);
  sciPrintText((unsigned char *)printBuff, strlen(printBuff), 0xFFFF);

  /* Read test value from FRAM */
  LOG_IF_ERROR_CODE(
      getPersistentDataByIndex(OBC_PERSIST_SECTION_ID_OBC_TIME, 0, (void *)&readTime, sizeof(obc_time_persist_data_t)));

  sprintf(printBuff, "Read %ld from FRAM.\n\r", readTime.unixTime);
  sciPrintText((unsigned char *)printBuff, strlen(printBuff), 0xFFFF);

  sprintf(printBuff, "====== Finished FRAM write/read test ======\n\r");
  sciPrintText((unsigned char *)printBuff, strlen(printBuff), 0xFFFF);

  while (1)
    ;
}
