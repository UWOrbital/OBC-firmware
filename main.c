#include "supervisor.h"
#include "obc_sci_io.h"

#include <FreeRTOS.h>
#include <os_task.h>

#include <sys_common.h>
#include <gio.h>

int main(void) {

    // Run hardware initialization code (TODO: refactor all this into one function call)
    gioInit();

    // Initialize bus mutexes
    initSciMutex();

    // The supervisor is the only task running initially.
    initSupervisor();

    vTaskStartScheduler();
}
