#include "obc_errors.h"
#include "obc_assert.h"
#include "obc_board_config.h"
#include "obc_logging.h"
#include "obc_sci_io.h"
#include "vn100_common.h"

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define VN100_BAUDRATE 115200U
#define MAX_COMMAND_SIZE 120U
#define TICK_TIMEOUT portMAX_DELAY

static obc_error_code_t isValidBaudRate(uint32_t baudRate);

void initVN100(void) {
  initSciMutex();

  sciSetBaudrate(UART_VN100_REG, VN100_BAUDRATE);

  VN100SetBaudrate(VN100_BAUDRATE);

  stopASCIIOuputs();

  startBinaryOutputs();

  resumeASYNC();
}

obc_error_code_t VN100resetModule() {
  obc_error_code_t errCode;
  unsigned char buf[] = "$VNRST*4D\r\n";
  RETURN_IF_ERROR_CODE(sciSendBytes(buf, sizeof(buf) + 1, TICK_TIMEOUT, UART_VN100_REG));
  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t isValidBaudRate(uint32_t baudRate) {
  uint32_t validBaudRate[] = {9600, 19200, 38400, 57600, 115200, 128000, 230400, 460800, 921600};
  uint32_t length = sizeof(validBaudRate) / sizeof(uint32_t);
  for (uint32_t i = 0; i < length; i++) {
    if (baudRate == validBaudRate[i]) {
      return OBC_ERR_CODE_SUCCESS;
    }
  }
  return OBC_ERR_CODE_INVALID_ARG;
}

obc_error_code_t VN100SetBaudrate(uint32_t baudrate) {
  isValidBaudRate(baudrate);

  char baud[7] = {'\0'};
  // Set to XX for now, means to ignore the checksum
  const char checksum[] = "*XX\r\n";
  const char base[] = "$VNWRG,05,";
  unsigned char buf[MAX_COMMAND_SIZE] = {'\0'};
  snprintf(baud, sizeof(baud), "%ld", baudrate);

  size_t len1 = strlen(base);
  size_t len2 = strlen(baud);
  size_t len3 = strlen(checksum);

  // Begin appending the command
  memcpy(buf, base, len1);
  memcpy(buf + len1, baud, len2);
  memcpy(buf + len1 + len2, checksum, len3);

  size_t numBytes = len1 + len2 + len3;

  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(sciSendBytes(buf, numBytes, TICK_TIMEOUT, UART_VN100_REG));
  sciSetBaudrate(UART_VN100_REG, baudrate);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t pauseASYNC() {
  obc_error_code_t errCode;
  unsigned char command[] = "$VNASY,0*XX\r\n";
  RETURN_IF_ERROR_CODE(sciSendBytes(command, sizeof(command), TICK_TIMEOUT, UART_VN100_REG));
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t resumeASYNC() {
  obc_error_code_t errCode;
  unsigned char command[] = "$VNASY,1*XX\r\n";
  RETURN_IF_ERROR_CODE(sciSendBytes(command, sizeof(command) + 1, TICK_TIMEOUT, UART_VN100_REG));
  return OBC_ERR_CODE_SUCCESS;
}
