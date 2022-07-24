#include "obc_sci_io.h"

#include <gio.h>
#include <sci.h>

int main(void) {
    // Initialize hardware.
    gioInit();
    sciInit();

    // Initialize the SCI mutex.
    initSciMutex();

    while(1) {
        // Send a string of text via SCI2.
        printTextSci(scilinREG, (unsigned char *)"Hello World!\r\n", 14);

        // Toggle the LED.
        gioToggleBit(gioPORTB, 1);
        
        // Simple delay.
        for (int i = 0; i < 1000000; i++) {
            // Do nothing.
        }
    }

}