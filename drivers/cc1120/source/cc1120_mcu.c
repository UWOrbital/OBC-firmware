#include "cc1120_mcu.h"
#include "obc_spi_io.h"
#include <stdio.h>

/**
 * @brief Simultaneously sends and receives a byte over CC1120 SPI interface
 * 
 * @param outb - Data to transfer 
 * @param inb - Buffer to store the received byte.
 * @return error code - An error code from obc_errors.h
 */
obc_error_code_t mcu_cc1120_spi_transfer(uint8_t outb, uint8_t *inb) {
    return spiTransmitAndReceiveByte(spiREG1, outb, inb);
}

/**
 * @brief Calls the correct CS assert function based on the MCU selected.
 * 
 * @return error code - An error code from obc_errors.h
 */
obc_error_code_t mcu_cc1120_cs_assert() {
    return assertChipSelect(gioPORTA, 0);
}

/**
 * @brief Calls the correct CS deassert function based on the MCU selected.
 * 
 * @return error code - An error code from obc_errors.h
 */
obc_error_code_t mcu_cc1120_cs_deassert() {
    return deassertChipSelect(gioPORTA, 0);
}
