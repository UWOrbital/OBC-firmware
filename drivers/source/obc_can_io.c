#include "obc_can_io.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_semphr.h>
#include <os_task.h>

#include <can.h>
#include <sys_common.h>

static SemaphoreHandle_t canMutex = NULL;
static StaticSemaphore_t canMutexBuffer;

void initCANMutex(void) {
    if(canMutex == NULL) {
        canMutex = xSemaphoreCreateMutexStatic(&canMutexBuffer);
    }

    configASSERT(canMutex);
}