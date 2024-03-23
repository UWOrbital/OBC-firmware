#include <FreeRTOS.h>

#include "os_task.h"
#include "obc_reset.h"
#include "obc_logging.h"

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
  LOG_FATAL_FROM_ISR("***********************STACK OVERFLOW DETECTED!!!!!!***********************");
}
