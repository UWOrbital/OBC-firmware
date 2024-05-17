#include "obc_adc_helper.h"
#include "obc_adc_helper.c"
#include "obc_sci_io.h"
#include "obc_errors.h"
#include "obc_print.h"

#include <FreeRTOS.h>
#include <os_task.h>
#include <sci.h>
#include <adc.h>

#include <sys_common.h>

static StaticTask_t taskBuffer;
static StackType_t taskStack[1024];

void vTask1(void *pvParameters) {
  obc_error_code_t errCode;
  sciPrintf("ADC Demo\r\n");

  errCode = initADCMutex();
  if (errCode != OBC_ERR_CODE_SUCCESS) {
    sciPrintf("Error initializing ADC: %d\r\n", errCode);
  }

  enum ADC_module_t adc = ADC1;
  enum ADC_group_t group = GROUP1;
  enum ADC_channel_t testChannel = ADC_CHANNEL_0;

  while (1) {
    float voltage;
    errCode = adcGetSingleData(adc, group, testChannel, &voltage, pdMS_TO_TICKS(1));
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
