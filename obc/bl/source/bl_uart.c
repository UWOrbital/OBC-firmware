#include "bl_uart.h"
#include "bl_time.h"
#include "obc_errors.h"
#include "sci.h"

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

/* DEFINES */
#define BL_UART_SCIREG_BAUD 115200U

/* TYPEDEFS */
typedef struct {
  uint32_t baud;
  sciBASE_t *sciReg;
} bl_uart_reg_config_t;

/* PUBLIC FUNCTIONS */
void blUartInit(void) {
  sciInit();

  sciSetBaudrate(UART_BL_REG, BL_UART_SCIREG_BAUD);
}

obc_error_code_t blUartReadBytes(uint8_t *buf, uint32_t numBytes, uint32_t timeout_ms) {
  if (buf == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  uint32_t timeout = blGetCurrentTick() + timeout_ms;
  uint32_t bytesRead = 0U;

  do {
    if (sciIsRxReady(UART_BL_REG) == SCI_RX_INT) {
      buf[bytesRead] = (uint8_t)sciReceiveByte(UART_BL_REG);

      // TODO: Figure out why the board sometimes receives 0x00 as the first byte
      if (bytesRead == 0 && buf[bytesRead] == 0) {
        bytesRead -= 1;
      }

      bytesRead++;

      // Reset the timeout each time a byte is received
      timeout = blGetCurrentTick() + timeout_ms + 1;
    }
  } while (blGetCurrentTick() <= timeout && bytesRead < numBytes);

  if (bytesRead < numBytes - 1) {
    return OBC_ERR_CODE_INCOMPLETE_MESSAGE;
  } else {
    return OBC_ERR_CODE_SUCCESS;
  }
}

void blUartWriteBytes(uint32_t numBytes, uint8_t *buf) { sciSend(UART_BL_REG, numBytes, buf); }
