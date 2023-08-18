#pragma once

#include "obc_errors.h"

#include <sci.h>
#include <stdint.h>

// ----------------------------------------- Functions ---------------------------------------- //

/**
 * @brief Send a string of text via UART_PRINT_REG.
 *
 * @param text The text to send.
 * @param length The length of the text to send.
 * @param sciReg Pointer to SCI register to print to
 * @param uartMutexTimeoutTicks The number of ticks to wait for the mutex to become available.
 * @return OBC_ERR_CODE_SUCCESS on success, else an error code
 */
obc_error_code_t sciPrintText(unsigned char *text, uint32_t length, TickType_t uartMutexTimeoutTicks);

/**
 * @brief Printf via UART_PRINT_REG.
 *
 * @param sciReg Pointer to SCI register to print to
 * @param s The format string
 * @param ... Arguments to use in format string
 * @return OBC_ERR_CODE_SUCCESS on success, else an error code
 */
obc_error_code_t sciPrintf(const char *s, ...);