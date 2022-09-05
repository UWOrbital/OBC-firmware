#include "interface_tests.h"
#include "obc_sci_io.h"
#include "obc_i2c_io.h"

#include "i2c.h"
#include "spi.h"
#include "sci.h"
#include "gio.h"
#include "adc.h"
#include "can.h"

#include <stdint.h>

void interface_init(void)
{
    // Initialize all interfaces
    i2cInit();
    spiInit();
    sciInit();
    gioInit();
    adcInit();
    canInit();
}

void test_gpio_out(gioPORT_t *port, uint32_t pin, uint32_t value)
{
    gioSetBit(port, pin, value);
}

uint8_t test_gpio_in(gioPORT_t *port, uint32_t pin)
{
    return gioGetBit(port, pin);    
}

uint16_t test_adc(adcBASE_t *adc, uint32_t channel)
{
    adcData_t adc1_data[24];
    adcData_t adc2_data[16];

    if (adc != adcREG1 && adc != adcREG2) {
        return 0;
    }

    adcStartConversion(adc, adcGROUP1);

    /* wait and read the conversion count */
    while((adcIsConversionComplete(adc, adcGROUP1)) == 0);

    if (adc == adcREG1) {
        adcGetData(adc, adcGROUP1, adc1_data);
        return adc1_data[channel].value;
    } else if (adc == adcREG2) {
        adcGetData(adc, adcGROUP1, adc2_data);
        return adc2_data[channel].value;
    }
    return 0;
}

uint8_t test_uart_rx(sciBASE_t *sci)
{
    uint8_t rx_data;
    rx_data = sciReceiveByte(sci);
    return rx_data;
}

void test_uart_tx(sciBASE_t *sci, uint8_t tx_data)
{
    sciSendByte(sci, tx_data);
}

uint8_t test_i2c_tx(i2cBASE_t *i2c, uint8_t sAddr, uint8_t data)
{
    return i2cSendTo(sAddr, 1, &data)
}