#include "obc_sci_io.h"

#include <FreeRTOS.h>
#include <os_task.h>

#include <sys_common.h>
#include <sys_core.h>
#include <gio.h>
#include <sci.h>
#include "vn100.h"

static StaticTask_t taskBuffer;
static StackType_t taskStack[1024];

void vTask1(void *pvParameter) {
  while (1) {
    vn_ymr_packet_t myPacket;
    readVN100(VN_YMR, &myPacket);

    sciPrintf("Yaw: %f \r\n", myPacket.yaw);
    sciPrintf("Pitch: %f \r\n", myPacket.pitch);
    sciPrintf("Roll: %f \r\n", myPacket.roll);

    sciPrintf("MagX: %f \r\n", myPacket.magX);
    sciPrintf("MagY: %f \r\n", myPacket.magY);
    sciPrintf("MagZ: %f \r\n", myPacket.magZ);

    sciPrintf("AccelX: %f \r\n", myPacket.accelX);
    sciPrintf("AccelY: %f \r\n", myPacket.accelY);
    sciPrintf("AccelZ: %f \r\n", myPacket.accelZ);

    sciPrintf("GyroX: %f \r\n", myPacket.gyroX);
    sciPrintf("GyorY: %f \r\n", myPacket.gyroY);
    sciPrintf("GyroZ: %f \r\n", myPacket.gyroZ);
    
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

int main(void) {
  // Run hardware initialization code
  gioInit();
  sciInit();

  _enable_interrupt_();

  // Initialize bus mutexes
  initSciMutex();
  initVN100();
  // Create a dummy task.
  BaseType_t xReturned;
  xReturned = xTaskCreateStatic(vTask1, "VN100 Demo", 1024, NULL, 1, taskStack, &taskBuffer);

  if (xReturned == pdPASS) {
    // Start the scheduler if the task was created successfully.
    vTaskStartScheduler();
  }
}
