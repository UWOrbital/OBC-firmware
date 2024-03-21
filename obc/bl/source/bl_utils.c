#include "bl_utils.h"
#include <stdint.h>

uint32_t computeCRC32Checksum(uint8_t* data, uint32_t length, uint32_t seed) {
  uint32_t crc = seed;
  while (length--) {
    crc = (crc << 8) ^ crc32_table[((crc >> 24) ^ *data) & 255];
    data++;
  }
  return crc;
}
