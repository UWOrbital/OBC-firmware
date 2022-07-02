#include "FreeRTOS.h"
#include "os_portmacro.h"
#include "os_task.h"

#ifndef POSIX_BUILD
#include "sys_common.h"
#include "gio.h"
#else
#include "console.h"
#endif

#include "supervisor.h"

int main(void) {

    #ifndef POSIX_BUILD
    // run hardware initialization code (TODO: refactor all this into one function call)
    gioInit();
    #else
    console_init();
    #endif

    // initialize important tasks
    xTaskHandle xSupervisorTaskHandle;
    xTaskCreate(vSupervisorTask, SUPERVISOR_NAME, SUPERVISOR_STACK_SIZE, NULL, SUPERVISOR_PRIORITY, &xSupervisorTaskHandle);

    // start task scheduler
    vTaskStartScheduler();
}
