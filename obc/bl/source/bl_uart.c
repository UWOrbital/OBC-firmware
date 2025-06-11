#include "bl_uart.h"
#include "rti.h"

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

void blUartReadBytes(uint8_t *buf, uint32_t numBytes, uint32_t timeout_ms) {
  uint32_t initTime = rtiGetCurrentTick(rtiCOMPARE1);
  do {
    if (sciIsRxReady(UART_BL_REG) == SCI_RX_INT) {
      for (uint32_t i = 0U; i < numBytes; i++) {
        buf[i] = (uint8_t)sciReceiveByte(UART_BL_REG);
      }
    }
  } while (((rtiGetCurrentTick(rtiCOMPARE1) - initTime) / 50) < timeout_ms ||
           rtiGetCurrentTick(rtiCOMPARE1) < initTime);
}

void blUartWriteBytes(uint32_t numBytes, uint8_t *buf) { sciSend(UART_BL_REG, numBytes, buf); }
