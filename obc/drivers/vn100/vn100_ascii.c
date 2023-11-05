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

/* ---------------------------- ASYNC Configuration Commands -------------------------- */
#define ASYNC_YPR "$VNWRG,06,1*XX\r\n"
#define ASYNC_MAG "$VNWRG,06,10*XX\r\n"
#define ASYNC_ACCEL "$VNWRG,06,11*XX\r\n"
#define ASYNC_GYRO "$VNWRG,06,12*XX\r\n"
#define ASYNC_YMR "$VNWRG,06,14*XX\r\n"

#define MUTEX_TIMEOUT portMAX_DELAY
#define MAX_SEND_SIZE 128U
#define MAX_RECEIVE_SIZE 120U

obc_error_code_t printSerialAscii(vn100_ascii_types_t cmd) {
  uint8_t len = 0;
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

  unsigned char buf[MAX_RECEIVE_SIZE] = {'\0'};
  obc_error_code_t errCode = sciReadBytes(buf, len, MUTEX_TIMEOUT, pdMS_TO_TICKS(1000), UART_VN100_REG);
  if (errCode == OBC_ERR_CODE_SUCCESS) {
    sciPrintText(buf, len, portMAX_DELAY);
    sciPrintf("\r\n");
  }
  return errCode;
}

obc_error_code_t startAsciiOutputs(vn100_ascii_types_t cmd) {
  obc_error_code_t errCode;
  unsigned char asyncCommand[MAX_SEND_SIZE];
  switch (cmd) {
    case VN_YPR: {
      memcpy(asyncCommand, ASYNC_YPR, sizeof(ASYNC_YPR));
      RETURN_IF_ERROR_CODE(sciSendBytes(asyncCommand, sizeof(ASYNC_YPR), MUTEX_TIMEOUT, UART_VN100_REG));
      break;
    }
    case VN_MAG: {
      memcpy(asyncCommand, ASYNC_MAG, sizeof(ASYNC_MAG));
      RETURN_IF_ERROR_CODE(sciSendBytes(asyncCommand, sizeof(ASYNC_MAG), MUTEX_TIMEOUT, UART_VN100_REG));
      break;
    }
    case VN_ACC: {
      memcpy(asyncCommand, ASYNC_ACCEL, sizeof(ASYNC_ACCEL));
      RETURN_IF_ERROR_CODE(sciSendBytes(asyncCommand, sizeof(ASYNC_ACCEL), MUTEX_TIMEOUT, UART_VN100_REG));
      break;
    }
    case VN_GYR: {
      memcpy(asyncCommand, ASYNC_GYRO, sizeof(ASYNC_GYRO));
      RETURN_IF_ERROR_CODE(sciSendBytes(asyncCommand, sizeof(ASYNC_GYRO), MUTEX_TIMEOUT, UART_VN100_REG));
      break;
    }
    case VN_YMR: {
      memcpy(asyncCommand, ASYNC_YMR, sizeof(ASYNC_YMR));
      RETURN_IF_ERROR_CODE(sciSendBytes(asyncCommand, sizeof(ASYNC_YMR), MUTEX_TIMEOUT, UART_VN100_REG));
      break;
    }
    default:
      return OBC_ERR_CODE_INVALID_ARG;
  }
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t stopAsciiOuputs(void) {
  unsigned char buf[] = "$VNWRG,06,0*XX\r\n";
  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(sciSendBytes(buf, sizeof(buf), portMAX_DELAY, UART_VN100_REG));
  return OBC_ERR_CODE_SUCCESS;
}
