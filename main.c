#include "gio.h"    // Imports the driver functions to interact with gio pins

int main(void) {
    gioInit();      // Initializes the gio Module
    while (1) {
        if (!gioGetBit(gioPORTB, 2)) {      // remember hardcoded pull up
            gioToggleBit(gioPORTB, 1);      // toggles the gio bit that controls the led
        }
    }
    return 0;
}
