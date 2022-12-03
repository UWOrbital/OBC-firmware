#ifndef DRIVERS_INCLUDE_OBC_SCI_IO_H_
#define DRIVERS_INCLUDE_OBC_SCI_IO_H_

#include "obc_errors.h"

#include <sci.h>
#include <stdint.h>

#ifdef RM46_LAUNCHPAD
	#define UART_PRINT_REG scilinREG 
#elif OBC_REVISION_1
	#define UART_PRINT_REG sciREG 
#elif OBC_REVISION_2
	#error Serial port not yet chosen for OBC_REVISION_2
#else
	#error Board not defined
#endif

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

#endif /* DRIVERS_INCLUDE_OBC_SCI_IO_H_ */
