#include <FreeRTOSConfig.h>
#include <FreeRTOS.h>
#include <os_task.h>

__attribute__((noreturn))
__attribute__((weak))
void freertosConfigAssert( const char * pcFile, unsigned long ulLine) { taskDISABLE_INTERRUPTS(); while(1); }
