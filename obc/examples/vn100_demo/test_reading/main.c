#include "obc_print.h"
#include "obc_sci_io.h"
#include "obc_board_config.h"

#include <FreeRTOS.h>
#include <os_task.h>

#include <sys_common.h>
#include <sys_core.h>
#include <gio.h>
#include <sci.h>

#define NUM_CHARS_TO_READ 120U
#define UART_MUTEX_BLOCK_TIME portMAX_DELAY

static StaticTask_t taskBuffer;
static StackType_t taskStack[1024];

void vTaskCode(void* pvParameters) {
  while (1) {
    unsigned char buffer[NUM_CHARS_TO_READ] = {'\0'};
    obc_error_code_t error =
        sciReadBytes(buffer, NUM_CHARS_TO_READ, UART_MUTEX_BLOCK_TIME, pdMS_TO_TICKS(1000), UART_VN100_REG);
    if (error != OBC_ERR_CODE_SUCCESS) {
      sciPrintf("Error reading from SCI! - %d\r\n", (int)error);
      continue;
    } else {
      sciPrintText(buffer, NUM_CHARS_TO_READ, UART_MUTEX_BLOCK_TIME);
      sciPrintf("\r\n");
      continue;
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
  sciSetBaudrate(UART_VN100_REG, 115200);
  // Assume all tasks are created correctly
  xTaskCreateStatic(vTaskCode, "Demo", 1024, NULL, 1, taskStack, &taskBuffer);

  vTaskStartScheduler();
}
