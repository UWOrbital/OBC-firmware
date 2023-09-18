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
    vn_ypr_packet_t myPacket;
    retrieveYPR(&myPacket);

    sciPrintf("Yaw: %f \r\n", myPacket.yaw);
    sciPrintf("Pitch: %f \r\n", myPacket.pitch);
    sciPrintf("Roll: %f \r\n", myPacket.roll);

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
