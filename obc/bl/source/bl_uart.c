#include "bl_uart.h"

#include <sci.h>
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

void blUartReadBytes(uint8_t *buf, uint32_t numBytes) {
  for (uint32_t i = 0U; i < numBytes; i++) {
    buf[i] = (uint8_t)sciReceiveByte(UART_BL_REG);
  }
}

void blUartWriteBytes(uint32_t numBytes, uint8_t *buf) { sciSend(UART_BL_REG, numBytes, buf); }
