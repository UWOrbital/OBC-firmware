#pragma once

#include "bl_errors.h"
#include <sci.h>
#include <stdint.h>

void bl_uart_init(void);
bl_error_code_t bl_uart_readBytes(sciBASE_t *sciReg, uint8_t *buf, uint32_t numBytes);
