#pragma once

#include "obc_board_config.h"
#include <stdint.h>

/**
 * @brief Initialize the UART module
 *
 */
void blUartInit(void);

/**
 * @brief Read a stream of bytes from the UART
 *
 * @param uartReg UART register to read from
 * @param buf Buffer to read into
 * @param numBytes Number of bytes to read
 */
void blUartReadBytes(uint8_t *buf, uint32_t numBytes);

/**
 * @brief Write a stream of bytes to the UART
 *
 * @param uartReg UART register to write to
 * @param buf Buffer to write from
 * @param numBytes Number of bytes to write
 */
void blUartWriteBytes(uint32_t numBytes, uint8_t *buf);
