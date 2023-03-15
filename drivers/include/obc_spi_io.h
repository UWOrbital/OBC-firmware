#ifndef DRIVERS_INCLUDE_OBC_SPI_IO_H_
#define DRIVERS_INCLUDE_OBC_SPI_IO_H_

#include "obc_errors.h"

#include <stdint.h>

#include <spi.h>
#include <gio.h>

/**
 * @brief Initialize mutexes protecting SPI ports.
 */
void initSpiMutex(void);

/**
 * @brief Deselect chip select; set the chip select pin high.
 * @param spiPort The SPI port to use.
 * @param csNum The chip select pin to use.
 * @return Error code. OBC_ERR_CODE_SUCCESS if successful.
 */
obc_error_code_t deassertChipSelect(gioPORT_t *spiPort, uint8_t csNum);

/**
 * @brief Select chip select; set the chip select pin low.
 * @param spiPort The SPI port to use.
 * @param csNum The chip select pin to use.
 * @return Error code. OBC_ERR_CODE_SUCCESS if successful.
 */
obc_error_code_t assertChipSelect(gioPORT_t *spiPort, uint8_t csNum);

/**
 * @brief Send and receive a byte via SPI.
 * @param spiReg The SPI register to use.
 * @param spiDataFormat The SPI data format options.
 * @param outb The byte to send.
 * @param inb Buffer to store the received byte.
 * @return Error code. OBC_ERR_CODE_SUCCESS if successful.
 */
obc_error_code_t spiTransmitAndReceiveByte(spiBASE_t *spiReg, spiDAT1_t *spiDataFormat, uint8_t outb, uint8_t *inb);

/**
 * @brief Send a byte via SPI.
 * 
 * @param spiReg The SPI register to use.
 * @param outb The byte to send.
 * @return Error code. OBC_ERR_CODE_SUCCESS if successful.
 */
obc_error_code_t spiTransmitByte(spiBASE_t *spiReg, spiDAT1_t *spiDataFormat, uint8_t outb);

/**
 * @brief Receive a byte via SPI.
 * 
 * @param spiReg The SPI register to use.
 * @param inb Buffer to store the received byte.
 * @return Error code. OBC_ERR_CODE_SUCCESS if successful.
 */
obc_error_code_t spiReceiveByte(spiBASE_t *spiReg, spiDAT1_t *spiDataFormat, uint8_t *inb);

#endif // DRIVERS_INCLUDE_OBC_SPI_IO_H_