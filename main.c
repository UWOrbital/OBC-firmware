#include "obc_logging.h"
#include "supervisor.h"
#include "obc_sci_io.h"
#include "obc_i2c_io.h"
#include "obc_spi_io.h"

#include <FreeRTOS.h>
#include <os_task.h>

#include <sys_common.h>
#include <gio.h>
#include <sci.h>
#include <i2c.h>
#include <spi.h>
#include <can.h>

#include "camera_reg.h"
#include "arducam.h"

int main(void) {

    // Run hardware initialization code (TODO: refactor all this into one function call)
    gioInit();
    sciInit();
    i2cInit();
    spiInit();
    canInit();

    // Initialize logger
    initLogger();

    // Initialize bus mutexes
    initSciMutex();
    initI2CMutex();
    initSpiMutex();

    uint8_t recv_data = 0;
    uint8_t send = 0;
    while(1) {
        for(int i = 0; i < 5; i++) {
            camReadReg(0x00, &recv_data, PRIMARY);
            for (int i = 0; i < 10000000; i++) { }
            camWriteReg(0x00, send, PRIMARY);
            for (int i = 0; i < 10000000; i++) { }
            send++;
            if(send < 5) {
                send = 0;
            }
        }

    }
}
