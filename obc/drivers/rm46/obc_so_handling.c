#include "os_task.h"
#include "obc_reset.h"

void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char *pcTaskName) {
  resetSystem(RESET_REASON_STACK_OVERFLOW_DETECTED);
}
