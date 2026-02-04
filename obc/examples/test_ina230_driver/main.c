#include "ina230.h"
#include "obc_sci_io.h"
#include "obc_i2c_io.h"
#include "obc_print.h"
#include "obc_board_config.h"

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
  float num = 100;
  obc_error_code_t errCode = 0;
  while (1) {
    errCode = getINA230ShuntVoltage(INA230_DEVICE_ONE, &num);
    if (errCode != OBC_ERR_CODE_SUCCESS) {
      sciPrintf("Error Reading Shunt Voltage - %d\r\n", (int)errCode);
    } else {
      sciPrintf("Shunt Voltage - %f\r\n", num);
    }

    errCode = getINA230Current(INA230_DEVICE_ONE, &num);
    if (errCode != OBC_ERR_CODE_SUCCESS) {
      sciPrintf("Error Reading Current - %d\r\n", (int)errCode);
    } else {
      sciPrintf("Current - %f\r\n", num);
    }

    errCode = getINA230BusVoltage(INA230_DEVICE_ONE, &num);
    if (errCode != OBC_ERR_CODE_SUCCESS) {
      sciPrintf("Error Reading Bus Voltage - %d\r\n", (int)errCode);
    } else {
      sciPrintf("Bus Voltage - %f\r\n", num);
    }

    errCode = getINA230Power(INA230_DEVICE_ONE, &num);
    if (errCode != OBC_ERR_CODE_SUCCESS) {
      sciPrintf("Error Reading Power - %d\r\n", (int)errCode);
    } else {
      sciPrintf("Power - %f\r\n", num);
    }
    gioToggleBit(STATE_MGR_DEBUG_LED_GIO_PORT, STATE_MGR_DEBUG_LED_GIO_BIT);

    // Simple delay.
    vTaskDelay(pdMS_TO_TICKS(1000));
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
  initINA230();

  // Assume all tasks are created correctly
  xTaskCreateStatic(vTaskCode, "Demo", 1024, NULL, 1, taskStack, &taskBuffer);

  vTaskStartScheduler();
}
