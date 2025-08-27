#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief generates a checksum using CRC16-CCITT algorithm
 *
 * @param data: the data that the checksum should be calculated over
 * @param dataLen: length of the data to calculate the checksum over
 *
 * @return calculated checksum
 */
uint16_t calculateCrc16Ccitt(const uint8_t *data, uint16_t dataLen);

uint32_t crc32(uint32_t crc, uint8_t *data, uint32_t dataLen);

#ifdef __cplusplus
}
#endif
