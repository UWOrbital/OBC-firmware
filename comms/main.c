#include "obc_logging.h"
#include "obc_sci_io.h"
#include "obc_spi_io.h"

#include <FreeRTOS.h>
#include <os_task.h>

#include <sys_common.h>
#include <gio.h>
#include <sci.h>
#include <spi.h>

int main(void) {
    gioInit();
    sciInit();
    spiInit();

    // Initialize logger
    initLogger();
    
    // Initialize bus mutexes
    initSciMutex();
    initSpiMutex();

    vTaskStartScheduler();
}