#include "supervisor.h"
#include "obc_sci_io.h"
#include "obc_i2c_io.h"

#include <FreeRTOS.h>
#include <os_task.h>

#include <sys_common.h>
#include <gio.h>
#include <sci.h>
#include <i2c.h>

#include <setup.h>

int main(void) {

    //initialize gioInit, sciInit, i2cInit, and baud rates. 
    funcSetUp();

    // Initialize bus mutexes
    initSciMutex();
    initI2CMutex();

    // The supervisor is the only task running initially.
    initSupervisor();

    vTaskStartScheduler();
}
