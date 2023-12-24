#pragma once

#include <stdint.h>

typedef enum {
  BL_UART_SCIREG_1 = 0,
  BL_UART_SCIREG_2 = 1,
} bl_uart_reg_t;

/**
 * @brief Initialize the UART module
 *
 */
void bl_uart_init(void);

/**
 * @brief Read a stream of bytes from the UART
 *
 * @param uartReg UART register to read from
 * @param buf Buffer to read into
 * @param numBytes Number of bytes to read
 */
void bl_uart_readBytes(bl_uart_reg_t uartReg, uint8_t *buf, uint32_t numBytes);

/**
 * @brief Write a stream of bytes to the UART
 *
 * @param uartReg UART register to write to
 * @param buf Buffer to write from
 * @param numBytes Number of bytes to write
 */
void bl_uart_writeBytes(bl_uart_reg_t uartReg, uint32_t numBytes, uint8_t *buf);
