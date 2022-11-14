#include "obc_sci_io.h"

#include "obc_spi_io.h"
#include "obc_sci_io.h"
#include "fram.h"

#include <gio.h>
#include <sci.h>
#include <spi.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main(void) {
    // Initialize hardware.
    gioInit();
    sciInit();
    spiInit();

    
    // Initialize the SCI mutex.
    initSciMutex();
    spiMutexInit();
    uint8_t chipID[9];
    char msg[50] = {0};
    while(1) {
        
        //framRead(NULL, chipID, 9, RDID);
        snprintf(msg, 50, "ID:%X %X %X %X %X %X %X %X %X\r\n", chipID[0], chipID[1], chipID[2], chipID[3], chipID[4], chipID[5], chipID[6], chipID[7], chipID[8]);
        // Note: This will send through the USB port on the LaunchPad
        printTextSci(scilinREG, (unsigned char *) msg, 50);

        // Toggle the LED.
        gioToggleBit(gioPORTB, 1);
        
        // Simple delay.
        for (int i = 0; i < 1000000; i++) {
            // Do nothing.
        }
    }

    return 0;
}