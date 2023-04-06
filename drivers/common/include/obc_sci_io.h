#ifndef DRIVERS_INCLUDE_OBC_SCI_IO_H_
#define DRIVERS_INCLUDE_OBC_SCI_IO_H_

#include "obc_errors.h"

#include <sci.h>
#include <stdint.h>

/**
 * @brief Initialize mutexes protecting SCI and SCI2.
 */
void initSciMutex(void);

/**
 * @brief Send a string of text via UART_PRINT_REG.
 * 
 * @param text The text to send.
 * @param length The length of the text to send.
 * @return OBC_ERR_CODE_SUCCESS on success, else an error code
 */
obc_error_code_t sciPrintText(unsigned char *text, uint32_t length);

/**
 * @brief Printf via UART_PRINT_REG.
 * 
 * @param s The format string
 * @param ... Arguments to use in format string
 * @return OBC_ERR_CODE_SUCCESS on success, else an error code
 */
obc_error_code_t sciPrintf(const char *s, ...);

/**
 * @brief Read a byte from UART_READ_REG by polling.
 * 
 * @param character The character that is read
 * @return OBC_ERR_CODE_SUCCESS on success OBC_ERR_CODE_INVALID_ARG or OBC_ERR_CODE_UNKOWN on fail
 */
obc_error_code_t sciReadByte(unsigned char *character);

/**
 * @brief Read a string from UART_READ_REG by polling and store it in the text buffer.
 * 
 * @param text The text that stores the characters read
 * @param length The number of bytes to read
 * @return OBC_ERR_CODE_SUCCESS on success OBC_ERR_CODE_INVALID_ARG or OBC_ERR_CODE_UNKOWN on fail
 * 
 * @note Bytes will be read until a newline character or (length - 1) characters are received.
 * A null terminator will be added to the end of the string.
 */
obc_error_code_t sciRead(unsigned char *text, uint32_t length);

#endif /* DRIVERS_INCLUDE_OBC_SCI_IO_H_ */
