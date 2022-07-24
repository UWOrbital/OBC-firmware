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
        // Send a string of text via SCI2 (aka LIN module configured as SCI interface).
        // Note: This will send through the USB port on the LaunchPad
        printTextSci(scilinREG, (unsigned char *)"Hello from SCILin!\r\n", 20);

        // Send a string of text via SCI
        // Note: This will send through the SCITX pin on the LaunchPad; you'll need to 
        // connect an external USB-TTY converter to the LaunchPad to see the text.
        printTextSci(sciREG, (unsigned char *)"Hello from SCI!\r\n", 17);

        // Toggle the LED.
        gioToggleBit(gioPORTB, 1);
        
        // Simple delay.
        for (int i = 0; i < 1000000; i++) {
            // Do nothing.
        }
    }

    return 0;
}