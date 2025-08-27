#include "obc_crc.h"

#include <stdint.h>
#include <string.h>

#include <stdbool.h>  // required by redutils.h
#include <redutils.h>

uint32_t computeCrc32(const uint32_t prevCrc32, const uint8_t* buffer, size_t len) {
  return RedCrc32Update(prevCrc32, buffer, len);
}
