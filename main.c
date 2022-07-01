#include "FreeRTOS.h"
#include "os_portmacro.h"
#include "os_task.h"
#include "sys_common.h"
#include "gio.h"
#include "sci.h"

#include "supervisor.h"
#include "obc_sci_io.h"

int main(void) {

    // run hardware initialization code (TODO: refactor all this into one function call)
    gioInit();
    sciInit();

    sci_mutex_init();

    // initialize important tasks
    xTaskHandle xSupervisorTaskHandle;
    xTaskCreate(vSupervisorTask, SUPERVISOR_NAME, SUPERVISOR_STACK_SIZE, NULL, SUPERVISOR_PRIORITY, &xSupervisorTaskHandle);

    // start task scheduler
    vTaskStartScheduler();
}
