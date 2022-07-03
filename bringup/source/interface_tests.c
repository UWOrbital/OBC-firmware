/**
 * @file interface_tests.c
 * @author Daniel Gobalakrishnan
 * @date 2022-07-03
 */

#include "interface_tests.h"

// #include "i2c.h"
// #include "spi.h"
// #include "mibspi.h"
// #include "uart.h"
// #include "sci.h"
#include "gio.h"
// #include "adc.h"
// #include "can.h"

void interface_init(void)
{
    // Initialize all interfaces
    // i2cInit();
    // spiInit();
    // mibspiInit();
    // uartInit();
    // sciInit();
    gioInit();
    // adcInit();
    // canInit();
}
