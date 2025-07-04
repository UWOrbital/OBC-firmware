#include "bl_uart.h"
#include "bl_time.h"

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
  uint32_t initTime = blGetCurrentTick();
  do {
    if (sciIsRxReady(UART_BL_REG) == SCI_RX_INT) {
      for (uint32_t i = 0U; i < numBytes; i++) {
        buf[i] = (uint8_t)sciReceiveByte(UART_BL_REG);
        // TODO: Figure out why the board sometimes receives 0x00 as the first byte
        if (i == 0 && buf[i] == 0) {
          i -= 1;
        }
      }
      return OBC_ERR_CODE_SUCCESS;
    }
  } while (((blGetCurrentTick() - initTime) / 50) < timeout_ms || blGetCurrentTick() < initTime);

  return OBC_ERR_CODE_UART_FAILURE;
}

void blUartWriteBytes(uint32_t numBytes, uint8_t *buf) { sciSend(UART_BL_REG, numBytes, buf); }
