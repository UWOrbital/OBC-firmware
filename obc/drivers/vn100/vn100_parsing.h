#pragma once

#include "obc_errors.h"
#include "vn100_binary.h"

typedef enum {
  HARD_FAULT = 1,
  SERIAL_BUFFER_OVERFLOW = 2,
  INVALID_CHECKSUM = 3,
  INVALID_COMMAND = 4,
  NOT_ENOUGH_PARAMETERS = 5,
  TOO_MANY_PARAMETERS = 6,
  INVALID_PARAMETER = 7,
  INVALID_REGISTER = 8,
  UNATHORIZED_ACCESS = 9,
  WATCHDOG_RESET = 10,
  OUTPUT_BUFFER_OVERFLOW = 11,
  INSUFFICIENT_BAUD_RATE = 12,
  ERROR_BUFFER_OVERFLOW = 255
} VN100_error_t;

/**
 * @brief Parse the packets into their respective packet types
 * @param cmd Command that denotes which packet type it is
 * @param packet Unparsed packet in the form of a string
 * @param parsedPacket Buffer with the correct packet type
 * @param error Error code in case an error occurs
 *
 * @return OBC_ERR_CODE_SUCCESS on success, else an error code
 */
obc_error_code_t parsePacket(unsigned char* packet, void* parsedPacket, VN100_error_t* error);

/**
 * @brief If there is an error thrown by the VN-100, parse the error message and return the corresponding error
 * @param packet Unparsed packet in the form of a string
 * @param error Error code in case an error occurs
 *
 * @return OBC_ERR_CODE_SUCCESS on success, else an error code
 */
obc_error_code_t recoverErrorCodeFromPacket(unsigned char* packet, VN100_error_t* error);
