#include "vn100_ascii.h"
#include "vn100_common.h"

#include "obc_errors.h"
#include "obc_assert.h"
#include "obc_board_config.h"
#include "obc_logging.h"
#include "obc_sci_io.h"
#include "obc_print.h"

#include "stdint.h"
#include "stdio.h"
#include "string.h"

/* ---------------------------- Incoming ASCII Byte Sizes ----------------------------------- */
#define YPR_PACKET_SIZE 36U
#define MAG_PACKET_SIZE 36U
#define ACCEL_PACKET_SIZE 36U
#define GYRO_PACKET_SIZE 36U
#define YMR_PACKET_SIZE 120U

/* ---------------------------- Read Register Commands ------------------------------------- */
#define YPR_REQUEST_CMD "$VNRRG,8*XX\r\n"
#define MAG_REQUEST_CMD "$VNRRG,17*XX\r\n"
#define ACCEL_REQUEST_CMD "$VNRRG,18*XX\r\n"
#define GYRO_REQUEST_CMD "$VNRRG,19*XX\r\n"
#define YMR_REQUEST_CMD "$VNRRG,27*XX\r\n"

/* ---------------------------- ASYNC Configuration Commands -------------------------- */
#define ASYNC_YPR "$VNWRG,06,1*XX\r\n"
#define ASYNC_MAG "$VNWRG,06,10*XX\r\n"
#define ASYNC_ACCEL "$VNWRG,06,11*XX\r\n"
#define ASYNC_GYRO "$VNWRG,06,12*XX\r\n"
#define ASYNC_YMR "$VNWRG,06,14*XX\r\n"

#define TICK_TIMEOUT portMAX_DELAY
#define MAX_COMMAND_SIZE 256U
#define DEFAULT_OUTPUT_RATE 10U

static obc_error_code_t isValidOutputRate(uint32_t outputRate);

static obc_error_code_t isValidOutputRate(uint32_t outputRate) {
  uint32_t validOutputRate[] = {1, 2, 4, 5, 10, 20, 25, 40, 50, 100, 200};
  uint32_t length = sizeof(validOutputRate) / sizeof(uint32_t);
  for (uint32_t i = 0; i < length; i++) {
    if (outputRate == validOutputRate[i]) {
      return OBC_ERR_CODE_SUCCESS;
    }
  }
  return OBC_ERR_CODE_INVALID_ARG;
}

obc_error_code_t printSerialASCII(vn_ascii_types_t cmd) {
  size_t len = 0;
  switch (cmd) {
    case VN_YPR:
      len = YPR_PACKET_SIZE;
      break;
    case VN_MAG:
      len = MAG_PACKET_SIZE;
      break;
    case VN_ACC:
      len = ACCEL_PACKET_SIZE;
      break;
    case VN_GYR:
      len = GYRO_PACKET_SIZE;
      break;
    case VN_YMR:
      len = YMR_PACKET_SIZE;
      break;
    default:
      return OBC_ERR_CODE_INVALID_ARG;
  }

  unsigned char buf[MAX_COMMAND_SIZE] = {'\0'};
  obc_error_code_t errCode = sciReadBytes(buf, len, TICK_TIMEOUT, pdMS_TO_TICKS(1000), UART_VN100_REG);
  if (errCode == OBC_ERR_CODE_SUCCESS) {
    sciPrintText(buf, len, portMAX_DELAY);
    sciPrintf("\r\n");
  }
  return errCode;
}

obc_error_code_t setASCIIOutputRate(uint32_t outputRate) {
  isValidOutputRate(outputRate);
  // Make the size of the string representation sufficiently large, use memcpy to append string onto req
  char freq[3];

  // Set to XX for now, means to ignore the checksum
  const char checksum[] = "*XX\r\n";
  const char base[] = "$VNWRG,07,";
  unsigned char req[MAX_COMMAND_SIZE];
  snprintf(freq, sizeof(freq), "%ld", outputRate);

  size_t len1 = strlen(base);
  size_t len2 = strlen(freq);
  size_t len3 = strlen(checksum);

  // Begin appending the command
  memcpy(req, base, len1);
  memcpy(req + len1, freq, len2);
  memcpy(req + len1 + len2, checksum, len3);

  size_t numBytes = len1 + len2 + len3 + 1;

  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(sciSendBytes(req, numBytes, TICK_TIMEOUT, UART_VN100_REG));
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t startASCIIOutputs(vn_ascii_types_t cmd) {
  obc_error_code_t errCode;
  unsigned char asyncCommand[MAX_COMMAND_SIZE];
  switch (cmd) {
    case VN_YPR: {
      memcpy(asyncCommand, ASYNC_YPR, sizeof(ASYNC_YPR));
      RETURN_IF_ERROR_CODE(sciSendBytes(asyncCommand, sizeof(ASYNC_YPR) + 1, TICK_TIMEOUT, UART_VN100_REG));
      break;
    }
    case VN_MAG: {
      memcpy(asyncCommand, ASYNC_MAG, sizeof(ASYNC_MAG));
      RETURN_IF_ERROR_CODE(sciSendBytes(asyncCommand, sizeof(ASYNC_YPR) + 1, TICK_TIMEOUT, UART_VN100_REG));
      break;
    }
    case VN_ACC: {
      memcpy(asyncCommand, ASYNC_ACCEL, sizeof(ASYNC_ACCEL));
      RETURN_IF_ERROR_CODE(sciSendBytes(asyncCommand, sizeof(ASYNC_YPR) + 1, TICK_TIMEOUT, UART_VN100_REG));
      break;
    }
    case VN_GYR: {
      memcpy(asyncCommand, ASYNC_GYRO, sizeof(ASYNC_GYRO));
      RETURN_IF_ERROR_CODE(sciSendBytes(asyncCommand, sizeof(ASYNC_YPR) + 1, TICK_TIMEOUT, UART_VN100_REG));
      break;
    }
    case VN_YMR: {
      memcpy(asyncCommand, ASYNC_YMR, sizeof(ASYNC_YMR));
      RETURN_IF_ERROR_CODE(sciSendBytes(asyncCommand, sizeof(ASYNC_YPR) + 1, TICK_TIMEOUT, UART_VN100_REG));
      break;
    }
    default:
      return OBC_ERR_CODE_INVALID_ARG;
  }
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t stopASCIIOuputs(void) {
  unsigned char buf[] = "$VNWRG,06,0*XX\r\n";
  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(sciSendBytes(buf, sizeof(buf) + 1, portMAX_DELAY, UART_VN100_REG));
  return OBC_ERR_CODE_SUCCESS;
}
