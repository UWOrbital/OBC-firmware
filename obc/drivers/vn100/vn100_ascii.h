#pragma once

#include "obc_errors.h"
#include <stdint.h>

typedef enum {
  VN_YPR,  // Get yaw, pitch and roll measurements
  VN_MAG,  // Get magnetic measurements
  VN_ACC,  // Get acceleration measurements
  VN_GYR,  // Get angular rate measurements
  VN_YMR   // Get all of the above
} vn100_ascii_types_t;

/**
 * @brief Wrapper to read from the VN100 UART register, used to read the currently configured ASYNC ouputs.
 *
 * @param cmd Specify which packet type you expect to receive
 * @return OBC_ERR_CODE_SUCCESS on success, else an error code
 */
obc_error_code_t printSerialAscii(vn100_ascii_types_t cmd);

/**
 * @brief Configure which packet type will be asyncronously outputted, note that this will also change the packet header
 * type
 *
 * @param cmd Specify which type of information you want to be outputted
 * @return OBC_ERR_CODE_SUCCESS on success, else an error code
 */
obc_error_code_t startAsciiOutputs(vn100_ascii_types_t cmd);

/**
 * @brief Stops ASCII Outputs on the current active serial port. Should be used if switching modes.
 *
 * @return OBC_ERR_CODE_SUCCESS on success, else an error code
 */
obc_error_code_t stopAsciiOuputs(void);
