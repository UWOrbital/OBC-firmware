#pragma once

#include "obc_errors.h"
#include "vn100.h"

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

obc_error_code_t parsePacket(vn_cmd_t cmd, unsigned char* packet, void* parsedPacket, VN100_error_t* error);

obc_error_code_t recoverErrorCodeFromPacket(unsigned char* packet, VN100_error_t* error);
