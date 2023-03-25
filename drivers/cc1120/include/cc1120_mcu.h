#ifndef DRIVERS_CC1120_INCLUDE_CC1120_MCU_H_
#define DRIVERS_CC1120_INCLUDE_CC1120_MCU_H_

#include "obc_errors.h"
#include "obc_logging.h"
#include "obc_spi_io.h"
#include <stdint.h>

#define CC1120_SPI_REG spiREG4
#define CC1120_SPI_PORT spiPORT4
#define CC1120_SPI_CS SPI_CS_NONE
#define CC1120_SPI_FMT SPI_FMT_0
#define CC1120_DEASSERT_RETURN_IF_ERROR_CODE(errCode) DEASSERT_RETURN_IF_ERROR_CODE(CC1120_SPI_PORT, CC1120_SPI_CS, errCode)

/**
 * @brief Simultaneously sends and receives a byte over CC1120 SPI interface
 * 
 * @param outb - Data to transfer 
 * @param inb - Buffer to store the received byte.
 * @return error code - An error code from obc_errors.h
 */
obc_error_code_t mcuCC1120SpiTransfer(uint8_t outb, uint8_t *inb);

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
