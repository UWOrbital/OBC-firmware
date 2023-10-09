#pragma once

#include "gs_errors.h"
#include <stdint.h>

gs_error_code_t transmitByteData(uint8_t* data, uint16_t dataLen);
gs_error_code_t recieveByteData(uint8_t* data, uint16_t* dataLen);
