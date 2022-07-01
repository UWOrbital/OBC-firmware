/**
 * @file obc_sci_io.h
 * @author Daniel Gobalakrishnan
 * @date 2022-07-01
 */

#ifndef OBC_SCI_COMMS_H
#define OBC_SCI_COMMS_H

#include "sci.h"
#include "stdint.h"

/**
 * @brief Initialize the SCI mutex.
 */
void sci_mutex_init(void);

/**
 * @brief Send a string of text via SCI2.
 * @param text The text to send.
 * @param length The length of the text to send.
 * @return 1 if the text was sent, 0 otherwise.
 */
uint8_t sci_print_text(uint8_t *text, uint32_t length);

#endif /* OBC_SCI_COMMS_H */