#include "obc_adc.h"
#include "obc_sci_io.h"
#include "obc_errors.h"
#include "obc_print.h"
#include "obc_scheduler_config.h"

#include <FreeRTOS.h>
#include <os_task.h>
#include <sci.h>
#include <adc.h>

#include <sys_common.h>
#include <sys_core.h>

static StaticTask_t taskBuffer;
static StackType_t taskStack[1024];

void vTask1(void *pvParameters) {
  obc_error_code_t errCode;
  sciPrintf("ADC Demo\r\n");

  errCode = initADCMutex();
  if (errCode != OBC_ERR_CODE_SUCCESS) {
    sciPrintf("Error initializing ADC: %d\r\n", errCode);
  }

  while (1) {
    float voltage;
    // Enums were not working, so i used the actual values. Signifies ADC1, Channel 0, Group1
    errCode = adcGetSingleData(0, 0, 1, &voltage, pdMS_TO_TICKS(1));
    if (errCode != OBC_ERR_CODE_SUCCESS) {
      sciPrintf("Error reading voltage: %d\r\n", errCode);
    } else {
      sciPrintf("Voltage: %f\r\n", voltage);
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

int main(void) {
  sciInit();
  adcInit();

  initSciPrint();

  xTaskCreateStatic(vTask1, "Demo", 1024, NULL, 1, taskStack, &taskBuffer);

  vTaskStartScheduler();

  while (1)
    ;
}
