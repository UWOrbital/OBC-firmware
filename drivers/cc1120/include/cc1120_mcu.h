#ifndef DRIVERS_CC1120_INCLUDE_CC1120_MCU_H_
#define DRIVERS_CC1120_INCLUDE_CC1120_MCU_H_

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
obc_error_code_t mcuCC1120SpiTransfer(uint8_t data, uint8_t *inb);

/**
 * @brief Calls the correct CS assert function based on the MCU selected.
 * 
 * @return error code - An error code from obc_errors.h
 */
obc_error_code_t mcuCC1120CSAssert(void);

/**
 * @brief Calls the correct CS deassert function based on the MCU selected.
 * 
 * @return error code - An error code from obc_errors.h
 */
obc_error_code_t mcuCC1120CSDeassert(void);

#endif /* DRIVERS_CC1120_INCLUDE_CC1120_MCU_H_ */
