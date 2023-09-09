#include "vn100_parsing.h"
#include "obc_logging.h"

#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define VN100_ERR_CODE_STRING "$VNERR,"
#define VALID_RESPONSE_STRING "$VNWRG,75"
#define SYNC_HEADER_LENGTH 1
typedef struct {
  uint8_t groups;
  uint16_t groupField;
} VN100_header_t;

typedef struct {
  float payload[MAX_PAYLOAD_SIZE];
} VN100_payload_t;

typedef struct {
  VN100_header_t header;
  VN100_payload_t data;
  uint16_t crc;
} VN100_decoded_packet_t;

static const uint8_t PAYLOAD_OFFSET = sizeof(VALID_RESPONSE_STRING);
static const uint16_t MAX_PAYLOAD_SIZE = sizeof(vn_ymr_packet_t);
static const char errorCodePacket[] = VN100_ERR_CODE_STRING;
static const uint8_t errorCodeIndex = sizeof(errorCodePacket) - 1;

static obc_error_code_t __decodePacket(vn_cmd_t cmd, unsigned char* packet, VN100_decoded_packet_t* parsedPacket);
static uint16_t calculateCRC(unsigned char data[], unsigned int length);
static obc_error_code_t recoverErrorCodeFromPacket(unsigned char* packet, VN100_error_t* error);

obc_error_code_t parsePacket(vn_cmd_t cmd, unsigned char* packet, void* parsedPacket, VN100_error_t* error) {
  if (packet == NULL || parsedPacket == NULL) return OBC_ERR_CODE_INVALID_ARG;

  /* Parsing for error */
  if (!memcmp(packet, errorCodePacket, errorCodeIndex)) {
    obc_error_code_t errCode = 0;
    RETURN_IF_ERROR_CODE(recoverErrorCodeFromPacket(packet, error));
    return OBC_ERR_CODE_VN100_RESPONSE_ERROR;
  }

  VN100_decoded_packet_t decodedPacket = {0};
  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(__decodePacket(cmd, packet, &decodedPacket));

  switch (cmd) {
    case VN_YPR:
      memcpy(parsedPacket, &decodedPacket.data, sizeof(vn_ypr_packet_t));
      break;
    case VN_MAG:
      memcpy(parsedPacket, &decodedPacket.data, sizeof(vn_mag_packet_t));
      break;
    case VN_ACC:
      memcpy(parsedPacket, &decodedPacket.data, sizeof(vn_accel_packet_t));
      break;
    case VN_GYR:
      memcpy(parsedPacket, &decodedPacket.data, sizeof(vn_gyro_packet_t));
      break;
    case VN_YMR:
      memcpy(parsedPacket, &decodedPacket.data, sizeof(vn_ymr_packet_t));
      break;
    default:
      return OBC_ERR_CODE_INVALID_ARG;
  }
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

static obc_error_code_t recoverErrorCodeFromPacket(unsigned char* packet, VN100_error_t* error) {
  if (packet == NULL || error == NULL) return OBC_ERR_CODE_INVALID_ARG;

  const unsigned char errorCode = packet[errorCodeIndex];
  if (!((errorCode <= INSUFFICIENT_BAUD_RATE) || (errorCode == ERROR_BUFFER_OVERFLOW))) {
    return OBC_ERR_CODE_VN100_PARSE_ERROR;
  }
  *error = errorCode;
  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t __decodePacket(vn_cmd_t cmd, unsigned char* packet, VN100_decoded_packet_t* parsedPacket) {
  if (packet == NULL || parsedPacket == NULL) return OBC_ERR_CODE_INVALID_ARG;

  unsigned char* payload = &packet[PAYLOAD_OFFSET];  // The main payload

  VN100_decoded_packet_t decodedPacket = {0};
  memcpy(&decodedPacket.header, payload, sizeof(decodedPacket.header));

  unsigned char* data = &payload[sizeof(decodedPacket.header)];
  uint16_t packetSize = 0;
  switch (cmd) {
    case VN_YPR:
      packetSize = sizeof(vn_ypr_packet_t);
      break;
    case VN_MAG:
      packetSize = sizeof(vn_mag_packet_t);
      break;
    case VN_ACC:
      packetSize = sizeof(vn_accel_packet_t);
      break;
    case VN_GYR:
      packetSize = sizeof(vn_gyro_packet_t);
      break;
    case VN_YMR:
      packetSize = sizeof(vn_ymr_packet_t);
      break;
    default:
      return OBC_ERR_CODE_INVALID_ARG;
  }

  memcpy(&decodedPacket.data, data, packetSize);
  uint16_t checksum = calculateCRC(&data[SYNC_HEADER_LENGTH], packetSize);

  memcpy(&decodedPacket.crc, &data[packetSize], sizeof(decodedPacket.crc));
  if (checksum != decodedPacket.crc) {
    return OBC_ERR_CODE_VN100_PARSE_ERROR;
  }

  *parsedPacket = decodedPacket;
  return OBC_ERR_CODE_SUCCESS;
}
