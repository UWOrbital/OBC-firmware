#include "bl_uart.h"

#include <sci.h>

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

void bl_uart_init(void) {
  sciInit();

  sciSetBaudrate(sciREG, 115200U);
  sciSetBaudrate(scilinREG, 115200U);
}

bl_error_code_t bl_uart_readBytes(sciBASE_t *sciReg, uint8_t *buf, uint32_t numBytes) {
  if (!(sciReg == scilinREG || sciReg == sciREG)) {
    return BL_ERR_CODE_INVALID_ARG;
  }

  if (buf == NULL || numBytes < 1) {
    return BL_ERR_CODE_INVALID_ARG;
  }

  for (uint32_t i = 0U; i < numBytes; i++) {
    buf[i] = (uint8_t)sciReceiveByte(sciReg);
  }

  return BL_ERR_CODE_SUCCESS;
}
