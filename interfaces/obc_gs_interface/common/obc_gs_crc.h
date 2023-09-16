#pragma once

#include <stdint.h>

/**
 * @brief generates a checksum using CRC16-CCITT algorithm
 *
 * @param data: the data that the checksum should be calculated over
 * @param dataLen: length of the data to calculate the checksum over
 *
 * @return calculated checksum
 */
uint16_t calculateCrc16Ccitt(const uint8_t *data, uint16_t dataLen);
