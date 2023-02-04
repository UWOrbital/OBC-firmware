#include "obc_sci_io.h"
#include "obc_errors.h"

#include <gio.h>
#include <sci.h>

#define NUM_CHARS_TO_READ 20U

int main(void) {
    // Initialize hardware.
    gioInit();
    sciInit();

    // Initialize the SCI mutex.
    initSciMutex();

    sciPrintf("Demo started\r\n");

    while(1) {      
        unsigned char buffer[NUM_CHARS_TO_READ] = {'\0'};
        obc_error_code_t error = sciRead(buffer, NUM_CHARS_TO_READ);
        if (error != OBC_ERR_CODE_SUCCESS) {
            sciPrintf("Error reading from SCI! - %d\r\n", (int)error);
            continue;
        }

        sciPrintText(buffer, NUM_CHARS_TO_READ);
        sciPrintf("\r\n");
        
        // Toggle the LED.
        gioToggleBit(gioPORTB, 1);
        
        // Simple delay.
        for (int i = 0; i < 1000000; i++) {
            // Do nothing.
        }
    }

    return 0;
}
