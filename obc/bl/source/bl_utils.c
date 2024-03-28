#include "bl_utils.h"
#include <stdint.h>

uint32_t computeCRC32Checksum(const uint8_t* s, uint16_t n) {
  uint32_t crc = 0xFFFFFFFF;

  for (uint16_t i = 0; i < n; i++) {
    char ch = s[i];
    for (uint16_t j = 0; j < 8; j++) {
      uint32_t b = (ch ^ crc) & 1;
      crc >>= 1;
      if (b) crc = crc ^ 0xEDB88320;
      ch >>= 1;
    }
  }

  return ~crc;
}
