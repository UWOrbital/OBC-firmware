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

static StaticTask_t taskBuffer;
static StackType_t taskStack[1024];

void vTaskCode(void* pvParameters) {
  // Move this back to main after bug with sciReadBytes & dabort_ is fixed
  initVN100();

  obc_error_code_t errCode;
  vn_binary_packet_t packet;

  sciPrintf("Beginning demo \r\n");

  while (1) {
    errCode = readVN100(VN_YMR, &packet);

    if (errCode != OBC_ERR_CODE_SUCCESS) {
      sciPrintf("Error reading from VN100 - %d\r\n", errCode);
    } else {
      sciPrintf("Yaw:   %f | MagX: %f | AccelX: %f | GyroX: %f \r\n", packet.yaw, packet.magX, packet.accelX,
                packet.gyroX);
      sciPrintf("Pitch: %f | MagY: %f | AccelY: %f | GyroY: %f \r\n", packet.yaw, packet.magX, packet.accelX,
                packet.gyroX);
      sciPrintf("Roll:  %f | MagZ: %f | AccelZ: %f | GyroZ: %f \r\n", packet.yaw, packet.magX, packet.accelX,
                packet.gyroX);
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