#pragma once

#include "obc_errors.h"

/**
 * @brief Release the mutex for the specified SPI port and chip select pin.
 *
 * @param spiReg The SPI register to use.
 *
 * @note This will not deassert the CS pin. This function is intended to be used
 * to send data to a device that requires the CS pin to be deasserted.
 *
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if successful, error code otherwise.
 */
obc_error_code_t dmaSpiReleaseBusMutex(spiBASE_t *spiReg);

/**
 * @brief Send a byte via SPI.
 * @param spiReg The SPI register to use.
 * @param spiDataFormat The SPI data format options.
 * @param outb The byte to send.
 * @return Error code. OBC_ERR_CODE_SUCCESS if successful.
 */
obc_error_code_t dmaSpiTransmitByte(spiBASE_t *spiReg, spiDAT1_t *spiDataFormat, uint8_t outb);

/**
 * @brief Send multiple bytes via SPI.
 * @param spiReg The SPI register to use.
 * @param spiDataFormat The SPI data format options.
 * @param outBytes The bytes to send.
 * @param numBytes The number of bytes to send.
 * @return Error code. OBC_ERR_CODE_SUCCESS if successful.
 */
obc_error_code_t dmaSpiTransmitBytes(spiBASE_t *spiReg, spiDAT1_t *spiDataFormat, uint8_t *outBytes, size_t numBytes);

/**
 * @brief Receive a byte via SPI.
 * @param spiReg The SPI register to use.
 * @param spiDataFormat The SPI data format options.
 * @param inb Buffer to store the received byte.
 * @return Error code. OBC_ERR_CODE_SUCCESS if successful.
 */
obc_error_code_t dmaSpiReceiveByte(spiBASE_t *spiReg, spiDAT1_t *spiDataFormat, uint8_t *inb);

/**
 * @brief Receive multiple bytes via SPI.
 * @param spiReg The SPI register to use.
 * @param spiDataFormat The SPI data format options.
 * @param inBytes Buffer to store the received byte.
 * @param numBytes The number of bytes to receive
 * @return Error code. OBC_ERR_CODE_SUCCESS if successful.
 */
obc_error_code_t dmaSpiReceiveBytes(spiBASE_t *spiReg, spiDAT1_t *spiDataFormat, uint8_t *inBytes, size_t numBytes);

/**
 * @brief Send and receive a byte via SPI.
 * @param spiReg The SPI register to use.
 * @param spiDataFormat The SPI data format options.
 * @param outb The byte to send.
 * @param inb Buffer to store the received byte.
 * @return Error code. OBC_ERR_CODE_SUCCESS if successful.
 */
obc_error_code_t dmaSpiTransmitAndReceiveByte(spiBASE_t *spiReg, spiDAT1_t *spiDataFormat, uint8_t outb, uint8_t *inb);

/**
 * @brief Send and receive multiple bytes via SPI.
 * @param spiReg The SPI register to use.
 * @param spiDataFormat The SPI data format options.
 * @param outBytes The byte to send.
 * @param inBytes Buffer to store the received byte.
 * @param numBytes The number of bytes to send and receive
 * @return Error code. OBC_ERR_CODE_SUCCESS if successful.
 */
obc_error_code_t dmaSpiTransmitAndReceiveBytes(spiBASE_t *spiReg, spiDAT1_t *spiDataFormat, uint8_t *outBytes,
                                               uint8_t *inBytes, size_t numBytes);

dmaWriteSpi()
