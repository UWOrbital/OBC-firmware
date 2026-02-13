#include "ina230.h"
#include "obc_sci_io.h"
#include "obc_i2c_io.h"
#include "obc_print.h"
#include "obc_board_config.h"
#include "tca6424.h"

#include <sci.h>
#include <stdio.h>
#include <gio.h>
#include <FreeRTOS.h>
#include <os_task.h>

#include <sys_common.h>
#include <sys_core.h>

static StaticTask_t taskBuffer;
static StackType_t taskStack[1024];

void vTaskCode(void* pvParameters) {
  obc_error_code_t errCode = 0;
  bool isToggled = true;
  while (1) {
    if (isToggled) {
      errCode = configureTCA6424APin(0x02, TCA6424A_GPIO_HIGH);
      sciPrintf("Toggled High\r\n");
      isToggled = false;
    } else {
      errCode = configureTCA6424APin(0x02, TCA6424A_GPIO_LOW);
      sciPrintf("Toggled Low\r\n");
      isToggled = true;
    }
    if (errCode != OBC_ERR_CODE_SUCCESS) {
      sciPrintf("Error Initializing - %d\r\n", (int)errCode);
    }
    sciPrintf("Success - %d\r\n", (int)errCode);
    gioToggleBit(STATE_MGR_DEBUG_LED_GIO_PORT, STATE_MGR_DEBUG_LED_GIO_BIT);

    // Simple delay.
    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}

int main(void) {
  // Run hardware initialization code
  gioInit();
  sciInit();
  i2cInit();

  sciEnableNotification(UART_PRINT_REG, SCI_RX_INT);

  _enable_interrupt_();

  // Initialize bus mutexes
  initSciPrint();
  initI2CMutex();


  // Assume all tasks are created correctly
  xTaskCreateStatic(vTaskCode, "Demo", 1024, NULL, 1, taskStack, &taskBuffer);

  vTaskStartScheduler();
}
