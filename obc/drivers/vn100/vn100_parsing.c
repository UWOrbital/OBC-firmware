#include <stdio.h>
#include <string.h>

#include "vn100_parsing.h"
#include "vn100_common.h"
#include "vn100_binary.h"
#include "obc_logging.h"

/*
#define DEAFULT_SYNC 0xFA
#define SYNC_HEADER_LENGTH 1
#define MAX_PAYLOAD_SIZE 10U  // Max size is defined by the largest packet ouputted
#define MAX_GROUP_FIELDS 10u
*/

#define PAYLOAD_OFFSET 14u

obc_error_code_t parsePacket(unsigned char* packet, vn_binary_packet_t* parsedPacket, VN100_error_t* error) {
  memcpy(parsedPacket, &packet[PAYLOAD_OFFSET], sizeof(vn_binary_packet_t));
  return OBC_ERR_CODE_SUCCESS;
}

/*
typedef struct {
  uint8_t groups;
  uint16_t groupField[MAX_PAYLOAD_SIZE]; // Could change in the future to include more groups
} VN100_decoded_header_t;

typedef struct {
  float payload[MAX_PAYLOAD_SIZE];
} VN100_decoded_payload_t;

typedef struct {
  VN100_header_t header;
  VN100_payload_t data;
  uint16_t crc;
} VN100_decoded_packet_t;

static uint8_t calculateSetBytes(uint8_t value);
static obc_error_code_t __decodePacket(unsigned char* packet, VN100_decoded_packet_t* parsedPacket);
static uint16_t calculateCRC(unsigned char data[], unsigned int length);

obc_error_code_t parsePacket(unsigned char* packet, void* parsedPacket, VN100_error_t* error) {
  if (packet == NULL || parsedPacket == NULL) return OBC_ERR_CODE_INVALID_ARG;

  if (!memcmp(packet, errorCodePacket, errorCodeIndex)) {
    obc_error_code_t errCode = 0;
    RETURN_IF_ERROR_CODE(recoverErrorCodeFromPacket(packet, error));
    return OBC_ERR_CODE_VN100_RESPONSE_ERROR;
  }

  VN100_decoded_packet_t decodedPacket = {0};
  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(__decodePacket(packet, &decodedPacket));

  memcpy(parsedPacket, &decodedPacket.data, sizeof(vn_binary_packet_t));
  return OBC_ERR_CODE_SUCCESS;
}

static uint16_t calculateCRC(unsigned char data[], unsigned int length) {
  unsigned int i;
  uint16_t crc = 0;

  for (i = 0; i < length; i++) {
    crc = (unsigned char)(crc >> 8) | (crc << 8);
    crc ^= data[i];
    crc ^= (unsigned char)(crc & 0xff) >> 4;
    crc ^= crc << 12;
    crc ^= (crc & 0x00ff) << 5;
  }
  return crc;
}

obc_error_code_t recoverErrorCodeFromPacket(unsigned char* packet, VN100_error_t* error) {
  if (packet == NULL || error == NULL) return OBC_ERR_CODE_INVALID_ARG;

  const unsigned char errorCode = packet[errorCodeIndex];
  if (!((errorCode <= INSUFFICIENT_BAUD_RATE) || (errorCode == ERROR_BUFFER_OVERFLOW))) {
    return OBC_ERR_CODE_VN100_PARSE_ERROR;
  }
  *error = errorCode;
  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t __decodePacket(unsigned char* packet, VN100_decoded_packet_t* parsedPacket) {
  if (packet == NULL || parsedPacket == NULL) return OBC_ERR_CODE_INVALID_ARG;

  VN100_header_t header = {};
  memcpy(&header.groups, packet, sizeof(header.groups));
  if (header.groups.sync != DEAFULT_SYNC) return OBC_ERR_CODE_VN100_PARSE_ERROR;

  header.groupCount = calculateSetBytes(header.groups.groups);
  memcpy(&header.groupField, packet + sizeof(header.groups) + header.groupCount, 2*header.groupCount);

  unsigned char* payload = packet + sizeof(header.groups) + header.groupCount;
  VN100_decoded_packet_t decodedPacket = {0};
  memcpy(&decodedPacket.header, payload, sizeof(decodedPacket.header));

  unsigned char* data = &payload[sizeof(decodedPacket.header)];

  uint16_t packetSize = 0;
  packetSize = sizeof(vn_binary_packet_t);
  return OBC_ERR_CODE_INVALID_ARG;

  memcpy(&decodedPacket.data, data, packetSize);
  uint16_t checksum = calculateCRC(&data[SYNC_HEADER_LENGTH], packetSize);

  memcpy(&decodedPacket.crc, &data[packetSize], sizeof(decodedPacket.crc));
  if (checksum != decodedPacket.crc) {
    return OBC_ERR_CODE_VN100_PARSE_ERROR;
  }

  *parsedPacket = decodedPacket;
  return OBC_ERR_CODE_SUCCESS;
}


static uint8_t calculateSetBytes(uint8_t value) {
  uint8_t count = 0;
  while (value != 0) {
    if (value & (0x01 << 7)) count++;
    value <<= 1;
  }
  return count;
}
*/
