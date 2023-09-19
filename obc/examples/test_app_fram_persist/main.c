#include "obc_sci_io.h"
#include "obc_print.h"
#include "obc_spi_io.h"
#include "obc_errors.h"
#include "obc_persistent.h"
#include "fm25v20a.h"

#include <FreeRTOS.h>
#include <os_task.h>

#include <sys_common.h>
#include <sci.h>
#include <spi.h>

static StaticTask_t taskBuffer;
static StackType_t taskStack[1024];

void vTask1(void *pvParameters) {
  obc_error_code_t errCode;
  sciPrintf("Persist Demo\r\n");

  obc_time_persist_data_t timeData = {0};
  timeData.unixTime = 0x12345678;

  errCode = setPersistentObcTime(&timeData);
  if (errCode != OBC_ERR_CODE_SUCCESS) {
    sciPrintf("Error setting time data: %d\r\n", errCode);
  }

  obc_time_persist_data_t readTimeData = {0};
  errCode = getPersistentObcTime(&readTimeData);
  if (errCode != OBC_ERR_CODE_SUCCESS) {
    sciPrintf("Error getting time data: %d\r\n", errCode);
  } else {
    sciPrintf("Time data: %x\r\n", readTimeData.unixTime);
  }

  sciPrintf("Corrupting FRAM\r\n");

  // Corrupt time data (As of 2023-05-28, address 0x9 is in the obc_time section of FRAM)
  uint8_t corrupt = 0xFF;
  uint32_t unixTimeAddr = OBC_PERSIST_ADDR_OF(obcTime.data);
  framWrite(unixTimeAddr, &corrupt, 1);

  errCode = getPersistentObcTime(&readTimeData);
  if (errCode != OBC_ERR_CODE_SUCCESS) {
    if (errCode == OBC_ERR_CODE_PERSISTENT_CORRUPTED) {
      sciPrintf("FRAM is corrupt\r\n");
    } else {
      sciPrintf("Error getting time data: %d\r\n", errCode);
    }
  } else {
    sciPrintf("Time data: %x\r\n", readTimeData.unixTime);
  }

  while (1)
    ;
}

int main(void) {
  sciInit();
  spiInit();

  initSciPrint();
  initSpiMutex();

  xTaskCreateStatic(vTask1, "Demo", 1024, NULL, 1, taskStack, &taskBuffer);

  vTaskStartScheduler();

  while (1)
    ;
}
