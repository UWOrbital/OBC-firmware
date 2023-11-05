#include "obc_errors.h"
#include "obc_assert.h"
#include "obc_board_config.h"
#include "obc_logging.h"
#include "obc_gs_crc.h"
#include "obc_sci_io.h"
#include "data_pack_utils.h"
#include "data_unpack_utils.h"

#include "vn100_binary.h"

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* -------------------------------------- Packet structure Byte Sizes -------------------------- */
#define BINARY_HEADER_SIZE 4U
#define BINARY_CRC_SIZE 2U
#define BINARY_PAYLOAD_SIZE 56U                                                        // Size excluding header and CRC
#define BINARY_PACKET_SIZE BINARY_CRC_SIZE + BINARY_PAYLOAD_SIZE + BINARY_HEADER_SIZE  // Total Size

/* -------------------------------------- Other relevant packet info -------------------------- */
#define DEFAULT_SYNC_BYTE 0xFA
#define SYNC_HEADER_LENGTH 1
#define PAYLOAD_OFFSET BINARY_HEADER_SIZE
#define VALID_CHECKSUM_RETURN 0
#define DEFAULT_BINARY_OUTPUT_RATE_HZ 10U
#define MAX_OUTPUT_RATE_SIZE 3U
#define MAX_SEND_SIZE 120U

/**
 * @brief Check the first pyte of the packet. 
 * @return If it is not 0xFA, return false (0) otherwise true (1).
 */
static uint8_t isSyncByteValid(unsigned char* packet);

/**
 * @brief Unpack buffer into a uin32_t value
 * @return The corresponding uint32_t value 
 */
static float unpackInt32LittleEndian(const uint8_t* buffer, uint32_t* offset);

/**
 * @brief Unpack buffer into a float value
 * @return The corresponding float value 
 */
static float unpackFloatLittleEndian(const uint8_t* buffer, uint32_t* offset);

/**
 * @brief Parse the packets into their respective packet types
 * @param cmd Command that denotes which packet type it is
 * @param packet Unparsed packet in the form of a string
 * @param parsedPacket Buffer with the correct packet type
 * @param error Error code in case an error occurs
 *
 * @return OBC_ERR_CODE_SUCCESS on success, else an error code
 */
static obc_error_code_t parsePacket(unsigned char* packet, vn100_binary_packet_t* parsedPacket, vn100_error_t* error);

static uint8_t isSyncByteValid(unsigned char* packet) {
  if (packet[0] == DEFAULT_SYNC_BYTE) {
    return 1;
  }
  return 0;
}

static float unpackInt32LittleEndian(const uint8_t* buffer, uint32_t* offset) {
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

static obc_error_code_t parsePacket(unsigned char* packet, vn100_binary_packet_t* parsedPacket, vn100_error_t* error) {
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

obc_error_code_t startBinaryOutputs(void) {
  /* Outputs: Yaw Pitch Roll, Angular rates, Accelerometer data, Magnetometer, Temp and Pressure. */
  // unsigned char buf[] = "$VNWRG,75,2,80,01,0528*XX\r\n";

  uint8_t outputRate = 800 / DEFAULT_BINARY_OUTPUT_RATE_HZ;  // See section

  const char header[] = "$VNWRG,75,2,";
  char outputRateString[MAX_OUTPUT_RATE_SIZE];  // Sufficient space for an int
  const char checksum[] = ",01,0528*XX\r\n";

  snprintf(outputRateString, sizeof(outputRateString), "%d", outputRate);

  size_t headerLength = strlen(header);
  size_t outputRateLength = strlen(outputRateString);
  size_t checksumLength = strlen(checksum);

  unsigned char buf[MAX_SEND_SIZE];

  // Begin appending the command
  memcpy(buf, header, headerLength);
  memcpy(buf + headerLength, outputRateString, outputRateLength);
  memcpy(buf + headerLength + outputRateLength, checksum, checksumLength);

  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(sciSendBytes(buf, sizeof(buf), portMAX_DELAY, UART_VN100_REG));
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t stopBinaryOutputs(void) {
  // unsigned char buf[] = "$VNWRG,75,0,80,01,0528*XX\r\n";

  uint8_t outputRate = 800 / DEFAULT_BINARY_OUTPUT_RATE_HZ;  // See section

  const char header[] = "$VNWRG,75,0,";
  char outputRateString[MAX_OUTPUT_RATE_SIZE];  // Sufficient space for an int
  const char checksum[] = ",01,0528*XX\r\n";

  snprintf(outputRateString, sizeof(outputRateString), "%d", outputRate);

  size_t headerLength = strlen(header);
  size_t outputRateLength = strlen(outputRateString);
  size_t checksumLength = strlen(checksum);

  unsigned char buf[MAX_SEND_SIZE];

  // Begin appending the command
  memcpy(buf, header, headerLength);
  memcpy(buf + headerLength, outputRateString, outputRateLength);
  memcpy(buf + headerLength + outputRateLength, checksum, checksumLength);

  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(sciSendBytes(buf, sizeof(buf), portMAX_DELAY, UART_VN100_REG));
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t readBinaryOutputs(vn100_binary_packet_t* parsedPacket) {
  unsigned char buf[BINARY_PACKET_SIZE] = {'\0'};
  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(sciReadBytes(buf, BINARY_PACKET_SIZE, portMAX_DELAY, pdMS_TO_TICKS(1000), UART_VN100_REG));

  vn100_error_t vnError;
  RETURN_IF_ERROR_CODE(parsePacket(buf, parsedPacket, &vnError));
  return OBC_ERR_CODE_SUCCESS;
}
