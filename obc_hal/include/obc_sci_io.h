#ifndef OBC_HAL_INCLUDE_OBC_SCI_IO_H_
#define OBC_HAL_INCLUDE_OBC_SCI_IO_H_

#include <sci.h>
#include <stdint.h>

/**
 * @brief Initialize the SCI mutex.
 */
void initSciMutex(void);

/**
 * @brief Send a string of text via SCI2.
 * @param sci The SCI register to use.
 * @param text The text to send.
 * @param length The length of the text to send.
 * @return 1 if the text was sent, 0 otherwise.
 */
uint8_t printTextSci(sciBASE_t *sci, unsigned char *text, uint32_t length);

#endif /* OBC_HAL_INCLUDE_OBC_SCI_IO_H_ */