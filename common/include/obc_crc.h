#ifndef COMMON_INCLUDE_OBC_CRC_H_
#define COMMON_INCLUDE_OBC_CRC_H_

#include <stdint.h>

#include "obc_errors.h"

obc_error_code_t computeCrc32(const uint8_t* data, uint32_t length, uint32_t* crc);

#endif // COMMON_INCLUDE_OBC_CRC_H_
