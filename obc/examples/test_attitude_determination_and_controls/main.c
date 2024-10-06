#include "attitude_control.h"
#include "attitude_determination_and_vehi.h"
#include "onboard_env_modelling_types.h"
#include "vn100.h"

#include <FreeRTOS.h>
#include <os_task.h>

#include <sys_common.h>
#include <sys_core.h>
#include <gio.h>
#include <sci.h>

static StaticTask_t taskBuffer;
static StackType_t taskStack[1024];

void readVn100(void* pvParameters) {
  initVn100();
  obc_error_code_t errCode;
  vn100_binary_packet_t packet;

  while (1) {
    errCode = vn100ReadBinaryOutputs(&packet);

    if (errCode != OBC_ERR_CODE_SUCCESS) {
      sciPrintf("Error reading from VN100 - %d\r\n", errCode);
    } else {
      sciPrintf("Yaw:   %f | MagX: %f | AccelX: %f | GyroX: %f \r\n", packet.yaw, packet.magX, packet.accelX,
                packet.gyroX);
      sciPrintf("Pitch: %f | MagY: %f | AccelY: %f | GyroY: %f \r\n", packet.pitch, packet.magY, packet.accelY,
                packet.gyroY);
      sciPrintf("Roll:  %f | MagZ: %f | AccelZ: %f | GyroZ: %f \r\n", packet.roll, packet.magZ, packet.accelZ,
                packet.gyroZ);
      sciPrintf("Temperature:  %f | Pressure: %f \r\n", packet.temp, packet.pres);
    }

    /* Set the input parameters for the first block */

    onboard_env_modelling_step();

    attitude_determination_and_vehi_step();

    attitude_control_step();

    // Toggle the LED.
    gioToggleBit(gioPORTB, 1);

    // Simple delay.
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void setOnBoardEnvInputs(ExtU_onboard_env_modelling_T* inputParams) { onboard_env_modelling_U = }

int main(void) {
  /* Initialize the entry point of the onboard modelling environment */
  onboard_env_modelling_initialize();

  /* Initialize the entry point of the attitude determination model */
  attitude_determination_and_vehi_initialize();

  /* Initialize the attitude control model */
  attitude_control_initialize();

  _enable_interrupt_();

  // Initialize bus mutexes
  initSciPrint();
  // Assume all tasks are created correctly
  xTaskCreateStatic(readVn100, "Demo", 1024, NULL, 1, taskStack, &taskBuffer);

  vTaskStartScheduler();
}
