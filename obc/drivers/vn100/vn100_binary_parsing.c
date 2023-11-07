#include "obc_errors.h"
#include "obc_gs_crc.h"

#include "vn100_binary_parsing.h"
#include "data_unpack_utils.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define DEFAULT_SYNC_BYTE 0xFA
#define SYNC_HEADER_LENGTH 1
#define PAYLOAD_OFFSET BINARY_HEADER_SIZE
#define VALID_CHECKSUM_RETURN 0

/**
 * @brief Check the first pyte of the packet.
 * @return If it is not 0xFA, return false (0) otherwise true (1).
 */
static uint8_t isSyncByteValid(unsigned char* packet);

/**
 * @brief Unpack buffer into a uin32_t value
 * @return The corresponding uint32_t value
 */
static uint32_t unpackInt32LittleEndian(const uint8_t* buffer, uint32_t* offset);

/**
 * @brief Unpack buffer into a float value
 * @return The corresponding float value
 */
static float unpackFloatLittleEndian(const uint8_t* buffer, uint32_t* offset);

static uint8_t isSyncByteValid(unsigned char* packet) {
  if (packet[0] == DEFAULT_SYNC_BYTE) {
    return 1;
  }
  return 0;
}

static uint32_t unpackInt32LittleEndian(const uint8_t* buffer, uint32_t* offset) {
  uint32_t value = ((uint32_t)buffer[*offset + 3] << 24) | ((uint32_t)buffer[*offset + 2] << 16) |
                   ((uint32_t)buffer[*offset + 1] << 8) | ((uint32_t)buffer[*offset]);
  (*offset) += 4;
  return value;
}

static float unpackFloatLittleEndian(const uint8_t* buffer, uint32_t* offset) {
  float val = 0.0;

  uint32_t tmp = unpackInt32LittleEndian(buffer, offset);
  memcpy(&val, &tmp, sizeof(val));

  return val;
}

obc_error_code_t parsePacket(unsigned char* packet, vn100_binary_packet_t* parsedPacket, vn100_error_t* error) {
  if (error == NULL || packet == NULL || parsedPacket == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  if (!isSyncByteValid(packet)) {
    return OBC_ERR_CODE_VN100_PARSE_ERROR;
  }

  /* The CRC is selected such that if you compute the 16-bit CRC starting with the group byte
  and include the CRC itself, a valid packet will result in 0x0000 computed
  by the running CRC calculation over the entire packet. */

  uint16_t vn100Crc = calculateCrc16Ccitt(&packet[1], BINARY_PACKET_SIZE - 1U);

  uint32_t offset = PAYLOAD_OFFSET;

  if (vn100Crc == VALID_CHECKSUM_RETURN) {
    parsedPacket->yaw = unpackFloatLittleEndian(packet, &offset);
    parsedPacket->pitch = unpackFloatLittleEndian(packet, &offset);
    parsedPacket->roll = unpackFloatLittleEndian(packet, &offset);

    parsedPacket->gyroX = unpackFloatLittleEndian(packet, &offset);
    parsedPacket->gyroY = unpackFloatLittleEndian(packet, &offset);
    parsedPacket->gyroZ = unpackFloatLittleEndian(packet, &offset);

    parsedPacket->accelX = unpackFloatLittleEndian(packet, &offset);
    parsedPacket->accelY = unpackFloatLittleEndian(packet, &offset);
    parsedPacket->accelZ = unpackFloatLittleEndian(packet, &offset);

    parsedPacket->magX = unpackFloatLittleEndian(packet, &offset);
    parsedPacket->magY = unpackFloatLittleEndian(packet, &offset);
    parsedPacket->magZ = unpackFloatLittleEndian(packet, &offset);

    parsedPacket->temp = unpackFloatLittleEndian(packet, &offset);
    parsedPacket->pres = unpackFloatLittleEndian(packet, &offset);

    return OBC_ERR_CODE_SUCCESS;
  }

  return OBC_ERR_CODE_VN100_CHECKSUM_ERROR;
}
