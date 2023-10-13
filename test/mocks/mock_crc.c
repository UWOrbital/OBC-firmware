#include "obc_crc.h"

#include "obc_errors.h"

#include <stdint.h>

// TODO: Implement this
uint32_t computeCrc32(const uint32_t prevCrc32, const uint8_t* buffer, size_t len) {
  if (buffer == NULL) return 0;

  return 1;
}
