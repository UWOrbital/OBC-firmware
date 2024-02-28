#include "obc_gs_crc.h"

#include <stdint.h>

uint16_t calculateCrc16Ccitt(const uint8_t *data, uint16_t dataLen) {
  // See VN100 user guide section 3.8.3 or ISO standard for CRC16-CCITT algorithm
  register uint16_t crc = 0;
  for (uint16_t i = 0; i < dataLen; ++i) {
    crc = (unsigned char)(crc >> 8) | (crc << 8);
    crc ^= data[i];
    crc ^= (unsigned char)(crc & 0xFF) >> 4;
    crc ^= crc << 12;
    crc ^= (crc & 0x00FF) << 5;
  }

  return crc;
}
