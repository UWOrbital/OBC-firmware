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

#define VN100_DEFAULT_BAUDRATE 115200U
#define MAX_SEND_SIZE 120U
#define MUTEX_TIMEOUT portMAX_DELAY
#define MAX_BAUDRATE_LENGTH 7U

static obc_error_code_t isValidBaudRate(uint32_t baudRate);

void initVN100(void) {
  sciSetBaudrate(UART_VN100_REG, VN100_DEFAULT_BAUDRATE);

  VN100SetBaudrate(VN100_DEFAULT_BAUDRATE);

  stopASCIIOuputs();

  startBinaryOutputs();

  resumeASYNC();
}

obc_error_code_t VN100resetModule() {
  obc_error_code_t errCode;
  unsigned char buf[] = "$VNRST*4D\r\n";
  RETURN_IF_ERROR_CODE(sciSendBytes(buf, sizeof(buf), MUTEX_TIMEOUT, UART_VN100_REG));
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
  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(isValidBaudRate(baudrate));

  char baud[MAX_BAUDRATE_LENGTH] = {'\0'};
  // Set to XX for now, means to ignore the checksum
  const char checksum[] = "*XX\r\n";
  const char header[] = "$VNWRG,05,";
  unsigned char buf[MAX_SEND_SIZE] = {'\0'};
  snprintf(baud, sizeof(baud), "%ld", baudrate);

  size_t headerLength = strlen(header);
  size_t baudrateLength = strlen(baud);
  size_t checksumLength = strlen(checksum);

  // Begin appending the command
  memcpy(buf, header, headerLength);
  memcpy(buf + headerLength, baud, baudrateLength);
  memcpy(buf + headerLength + baudrateLength, checksum, checksumLength);

  size_t numBytes = headerLength + baudrateLength + checksumLength;

  RETURN_IF_ERROR_CODE(sciSendBytes(buf, numBytes, MUTEX_TIMEOUT, UART_VN100_REG));
  sciSetBaudrate(UART_VN100_REG, baudrate);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t pauseASYNC() {
  obc_error_code_t errCode;
  unsigned char command[] = "$VNASY,0*XX\r\n";
  RETURN_IF_ERROR_CODE(sciSendBytes(command, sizeof(command), MUTEX_TIMEOUT, UART_VN100_REG));
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t resumeASYNC() {
  obc_error_code_t errCode;
  unsigned char command[] = "$VNASY,1*XX\r\n";
  RETURN_IF_ERROR_CODE(sciSendBytes(command, sizeof(command), MUTEX_TIMEOUT, UART_VN100_REG));
  return OBC_ERR_CODE_SUCCESS;
}
