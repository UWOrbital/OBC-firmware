#pragma once

#include "obc_errors.h"
#include "spi.h"

/**
 * @brief initalizes DMA for facilitating spi transfers for spiReg
 *
 * @param SpiReg SPI port that the DMA should be enabled for
 *
 * @return obc_error_code_t - whether or not the initialization was successful
 */
obc_error_code_t spiDmaInit(spiBASE_t *spiReg);

/**
 * @brief Uses the DMA to transmit and receive bytes over SPI
 *
 * @param spiReg the SPI bus to use for the transfer
 * @param spiDataFormat the spi data format to use for the transfer
 * @param txData array of uint16_t to be sent over the SPI bus
 * @param rxData buffer of atleast dataLen size to hold the received bytes
 * @param dataLen number of uint16_t to send
 *
 * @return obc_error_code_t - whether or not the transfer was successful
 */
obc_error_code_t dmaSpiTransmitandReceiveBytes(spiBASE_t *spiReg, spiDAT1_t *spiDataFormat, uint16_t *txData,
                                               uint16_t *rxData, size_t dataLen);

/**
 * @brief callback function to be called from ISR when DMA Block transfer is complete
 */
void dmaSpi1FinishedCallback(void);
