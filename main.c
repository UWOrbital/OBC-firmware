#include "FreeRTOS.h"
#include "os_portmacro.h"
#include "os_task.h"
#include "sys_common.h"
#include "gio.h"

#include "supervisor.h"

int main(void) {

    // run hardware initialization code (TODO: refactor all this into one function call)
    gioInit();

    // initialize important tasks
    xTaskHandle xSupervisorTaskHandle;
    xTaskCreate(vSupervisorTask, SUPERVISOR_NAME, SUPERVISOR_STACK_SIZE, NULL, SUPERVISOR_PRIORITY, &xSupervisorTaskHandle);

    // start task scheduler
    vTaskStartScheduler();
}