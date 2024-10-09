#include <FreeRTOSConfig.h>
#include <FreeRTOS.h>
#include <sys_selftest.h>

#include "os_task.h"
#include "obc_reset.h"
#include "obc_logging.h"

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
  LOG_FATAL_FROM_ISR("***********************STACK OVERFLOW DETECTED!!!!!!***********************");
}

__attribute__((noreturn)) void freertosConfigAssert(const char *pcFile, unsigned long ulLine) {
  logErrorCode(LOG_FATAL, pcFile, ulLine, OBC_ERR_CODE_FREERTOS_ASSERT_FAIL);

  vTaskDelay(15);

  resetSystem(RESET_REASON_FREERTOS_ASSERT_FAIL);
}

void _custom_dabort(void) {
  logErrorCodeFromISR(LOG_FATAL, __FILE__, 10, OBC_ERR_CODE_FREERTOS_ASSERT_FAIL);

  vTaskDelay(15);

  resetSystem(RESET_REASON_FREERTOS_ASSERT_FAIL);
}
