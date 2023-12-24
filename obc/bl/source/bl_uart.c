#include "bl_uart.h"

#include <sci.h>

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

/* DEFINES */
#define BL_UART_SCIREG_1_BAUD 115200U
#define BL_UART_SCIREG_2_BAUD 115200U

/* TYPEDEFS */
typedef struct {
  uint32_t baud;
  sciBASE_t *sciReg;
} bl_uart_reg_config_t;

/* PRIVATE VARIABLES */
static const bl_uart_reg_config_t bl_uart_reg_config[] = {
    [BL_UART_SCIREG_1] = {BL_UART_SCIREG_1_BAUD, sciREG},
    [BL_UART_SCIREG_2] = {BL_UART_SCIREG_2_BAUD, scilinREG},
};

/* PUBLIC FUNCTIONS */
void bl_uart_init(void) {
  sciInit();

  sciSetBaudrate(bl_uart_reg_config[BL_UART_SCIREG_1].sciReg, bl_uart_reg_config[BL_UART_SCIREG_1].baud);
  sciSetBaudrate(bl_uart_reg_config[BL_UART_SCIREG_2].sciReg, bl_uart_reg_config[BL_UART_SCIREG_2].baud);
}

void bl_uart_readBytes(bl_uart_reg_t reg, uint8_t *buf, uint32_t numBytes) {
  for (uint32_t i = 0U; i < numBytes; i++) {
    buf[i] = (uint8_t)sciReceiveByte(bl_uart_reg_config[reg].sciReg);
  }
}

void bl_uart_writeBytes(bl_uart_reg_t reg, uint32_t numBytes, uint8_t *buf) {
  sciSend(bl_uart_reg_config[reg].sciReg, numBytes, buf);
}
