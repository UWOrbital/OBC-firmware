#include "obc_logging.h"
#include "obc_adc.h"
#include "obc_sci_io.h"
#include "obc_errors.h"
#include "obc_print.h"
#include "obc_scheduler_config.h"
#include "metadata.h"

#include <FreeRTOS.h>
#include <os_task.h>
#include <sci.h>
#include <adc.h>

#include <sys_common.h>
#include <sys_core.h>

static StaticTask_t taskBuffer;
static StackType_t taskStack[1024];

extern uint32_t __metadata_start__;

void vTask1(void *pvParameters) {
  while (1) {
    metadata_struct_t metadata = {0};
    obc_error_code_t errCode;
    errCode = readAppMetadata(&metadata);

    if (errCode == OBC_ERR_CODE_NO_METADATA_APP_NOT_OVER_SERIAL) {
      sciPrintf("\r\nNO METADATA, APP NOT SENT TO BL OVER SERIAL\r\n");
    } else {
      sciPrintf("\r\nParsed values:\r\n");
      sciPrintf("Version: 0x%x\r\n", metadata.vers);
      sciPrintf("Binary Size: 0x%x\r\n", metadata.bin_size);
      vTaskDelay(1000);
    }
  }
}

int main(void) {
  sciInit();
  initSciPrint();

  xTaskCreateStatic(vTask1, "Demo", 1024, NULL, 1, taskStack, &taskBuffer);

  vTaskStartScheduler();

  while (1)
    ;
}
