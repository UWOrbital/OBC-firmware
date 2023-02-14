#ifndef CC1120_MCU_H
#define CC1120_MCU_H

#include "obc_errors.h"
#include <stdarg.h>
#include <stdint.h>

/**
 * @brief Simultaneously sends and receives a byte over CC1120 SPI interface
 * 
 * @param outb - Data to transfer 
 * @param inb - Buffer to store the received byte.
 * @return error code - An error code from obc_errors.h
 */
obc_error_code_t mcu_cc1120_spi_transfer(uint8_t data, uint8_t *inb);

/**
 * @brief Calls the correct CS assert function based on the MCU selected.
 * 
 * @return error code - An error code from obc_errors.h
 */
obc_error_code_t mcu_cc1120_cs_assert();

/**
 * @brief Calls the correct CS deassert function based on the MCU selected.
 * 
 * @return error code - An error code from obc_errors.h
 */
obc_error_code_t mcu_cc1120_cs_deassert();

#endif /* CC1120_MCU_H */
