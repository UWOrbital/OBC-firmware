#include "obc_print.h"
#include "obc_sci_io.h"
#include "obc_board_config.h"
#include "vn100_common.h"

#include <FreeRTOS.h>
#include <os_task.h>

#include <sys_common.h>
#include <sys_core.h>
#include <gio.h>
#include <sci.h>

#define DEFUALT_OUPUT_RATE 10

static StaticTask_t taskBuffer;
static StackType_t taskStack[1024];

void vTaskCode(void* pvParameters) {
  /* Disable binary outputs */
  stopBinaryOutputs();
  startASCIIOutputs(VN_YMR);
  setASCIIOutputRate(DEFUALT_OUPUT_RATE);

  while (1) {
    obc_error_code_t errCode = printSerialASCII(VN_YMR);
    
    if (errCode != OBC_ERR_CODE_SUCCESS) {
      sciPrintf("Error Code - %d\r\n", errCode);
    }

    // Toggle the LED.
    gioToggleBit(gioPORTB, 1);

    // Simple delay.
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

int main(void) {
  // Run hardware initialization code
  gioInit();
  sciInit();

  _enable_interrupt_();

  // Initialize bus mutexes
  initSciPrint();
  // Assume all tasks are created correctly
  xTaskCreateStatic(vTaskCode, "Demo", 1024, NULL, 1, taskStack, &taskBuffer);

  vTaskStartScheduler();
}
