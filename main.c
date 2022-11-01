#include "supervisor.h"
#include "obc_sci_io.h"
#include "obc_i2c_io.h"
#include "ds3232_mz.h"

#include <FreeRTOS.h>
#include <os_task.h>

#include <sys_common.h>
#include <gio.h>
#include <sci.h>
#include <i2c.h>

int main(void) {

    // Run hardware initialization code (TODO: refactor all this into one function call)
    gioInit();
    sciInit();
    i2cInit();

    // Initialize bus mutexes
    initSciMutex();
    initI2CMutex();

    // The supervisor is the only task running initially.
    initSupervisor();

    vTaskStartScheduler();
}
