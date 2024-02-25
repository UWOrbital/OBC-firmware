#include <FreeRTOS.h>
#include <stdint.h>
#include <sci.h>

#define stackSize 1024

static void vTask(void* param);
static StackType_t taskStackOne[stackSize];
static StackType_t taskStackTwo[stackSize];
static StaticTask_t taskStruct[2];

int main(void) {
  sciInit();
  initSciMutex();

  sciPrintf("Beginning Runtime stats test \r\n");

  xTaskHandle taskOneHandle, taskTwoHandle;

  if (xTaskCreateStatic(vTask, "Task One", stackSize, (void*)1, tskIDLE_PRIORITY, taskStackOne, &taskStruct[0]) !=
      pdPASS)
    goto fail;
  if (xTaskCreateStatic(vTask, "Task Two", stackSize, (void*)1, tskIDLE_PRIORITY, taskStackTwo, &taskStruct[1]) !=
      pdPASS)
    goto fail;
  vTaskStartScheduler();

fail:
  sciPrintf("Initalization of some thread or scheduler failed");
}

static void vTask(void* param) {
  static uint32_t counter = 0;
  while (1) {
    sciPrintf("Entered the task for entry: %d", counter);
    vTaskDelay(500);
  }
}
