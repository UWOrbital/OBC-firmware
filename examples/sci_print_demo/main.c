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
        // Send a string of text via SCI
        sciPrintText((unsigned char *)"Hello from SCI!\r\n", 20);
        
        // Test sciPrintf
        sciPrintf("Testing sciPrintf: %d %d %s\r\n", 0, 1, "Hello");

        // Toggle the LED.
        gioToggleBit(gioPORTB, 1);
        
        // Simple delay.
        for (int i = 0; i < 1000000; i++) {
            // Do nothing.
        }
    }

    return 0;
}
