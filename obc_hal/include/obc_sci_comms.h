#ifndef OBC_SCI_COMMS_H
#define OBC_SCI_COMMS_H

#include "sci.h"
#include "stdint.h"

void sci_mutex_init(void);

uint8_t sci_send_text(sciBASE_t *sci, uint8_t *text, uint32_t length);

#endif