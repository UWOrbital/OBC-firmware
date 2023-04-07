#include "test_gio.h"
#include "obc_sci_io.h"
#include "gio.h"

void testGIO(void) {
    sciPrintf("Testing GIO...\r\n");
    for(int i = 0; i < 8; i++){
        gioSetBit(gioPORTA, i, 1);
        gioSetBit(gioPORTB, i, 1);
    }
    sciPrintf("Setting bit 0 to 7 on both portA and portB to high\r\n");
    
}
