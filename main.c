#include "supervisor.h"

#include <FreeRTOS.h>
#include <os_task.h>

#include <sys_common.h>
#include <gio.h>

int main(void) {

    // run hardware initialization code (TODO: refactor all this into one function call)
    gioInit();

    // The supervisor is, initially, the only task running.
    initSupervisor();

    // start task scheduler
    vTaskStartScheduler();
}
