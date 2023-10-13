#include "obc_errors.h"
#include "obc_assert.h"
#include "obc_board_config.h"
#include "obc_logging.h"
#include "obc_sci_io.h"

#include "vn100_parsing.h"

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* ---------------------------- Incoming Binary Packet Byte Sizes ----------------------------------- */
#define BINARY_PACKET_SIZE 48U
#define MAX_PACKET_SIZE 256U

obc_error_code_t startBinaryOutputs(void) {
  /* Outputs: Yaw Pitch Roll, Angular rates, Accelerometer data, Magnetometer, Temp and Pressure. */
  unsigned char buf[] = "$VNWRG,75,2,80,01,0528*XX\r\n";
  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(sciSendBytes(buf, sizeof(buf), portMAX_DELAY, UART_VN100_REG));
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t stopBinaryOutputs(void) {
  unsigned char buf[] = "$VNWRG,75,0,80,01,0528*XX\r\n";
  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(sciSendBytes(buf, sizeof(buf), portMAX_DELAY, UART_VN100_REG));
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t readBinaryOutputs(void* parsedPacket) {
  unsigned char buf[MAX_PACKET_SIZE] = {'\0'};
  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(sciReadBytes(buf, BINARY_PACKET_SIZE, portMAX_DELAY, pdMS_TO_TICKS(10), UART_VN100_REG));

  VN100_error_t vnError;
  RETURN_IF_ERROR_CODE(parsePacket(buf, parsedPacket, &vnError));
  return OBC_ERR_CODE_SUCCESS;
}
