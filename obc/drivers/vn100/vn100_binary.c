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

/* -------------------------------------- Packet structure sizes -------------------------- */
#define BINARY_HEADER_SIZE 4U
#define BINARY_CRC_SIZE 2U
#define BINARY_PAYLOAD_SIZE 56U  // Size excluding header and CRC
#define BINARY_PACKET_SIZE BINARY_CRC_SIZE + BINARY_PAYLOAD_SIZE + BINARY_HEADER_SIZE

/* -------------------------------------- Other relevant packet info -------------------------- */
#define DEFAULT_SYNC_BYTE 0xFA
#define SYNC_HEADER_LENGTH 1
#define PAYLOAD_OFFSET BINARY_HEADER_SIZE
#define VALID_CHECKSUM_RETURN 0
#define DEFAULT_BINARY_OUTPUT_RATE_HZ 10U

// static void concatenateStrings(const char* str1, const char* str2, char* result, size_t resultSize);

// // Function to concatenate two strings without using malloc
// static void concatenateStrings(const char* str1, const char* str2, char* result, size_t resultSize) {
//     // Check if the result buffer is large enough
//     if (resultSize <= (strlen(str1) + strlen(str2)) + 1) {
//         return;
//     }

//     // Copy the first string into the result
//     strcpy(result, str1);

//     // Concatenate the second string to the end of the result
//     strcat(result, str2);
// }

/**
 * @brief Check the first pyte of the packet. If it is not 0xFA, return an error code.
 * @return OBC_ERR_CODE_SUCCESS on success, else an error code
 */
static uint8_t isSyncByteValid(unsigned char* packet);

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
    parsedPacket->yaw = unpackFloat(packet, &offset);
    parsedPacket->pitch = unpackFloat(packet, &offset);
    parsedPacket->roll = unpackFloat(packet, &offset);

    parsedPacket->gyroX = unpackFloat(packet, &offset);
    parsedPacket->gyroY = unpackFloat(packet, &offset);
    parsedPacket->gyroZ = unpackFloat(packet, &offset);

    parsedPacket->accelX = unpackFloat(packet, &offset);
    parsedPacket->accelY = unpackFloat(packet, &offset);
    parsedPacket->accelZ = unpackFloat(packet, &offset);

    parsedPacket->magX = unpackFloat(packet, &offset);
    parsedPacket->magY = unpackFloat(packet, &offset);
    parsedPacket->magZ = unpackFloat(packet, &offset);

    parsedPacket->temp = unpackFloat(packet, &offset);
    parsedPacket->pres = unpackFloat(packet, &offset);

    return OBC_ERR_CODE_SUCCESS;

    // memcpy(parsedPacket, &packet[PAYLOAD_OFFSET], sizeof(vn100_binary_packet_t));
    // return OBC_ERR_CODE_SUCCESS;
  }

  return OBC_ERR_CODE_VN100_CHECKSUM_ERROR;
}

obc_error_code_t startBinaryOutputs(void) {
  /* Outputs: Yaw Pitch Roll, Angular rates, Accelerometer data, Magnetometer, Temp and Pressure. */
  unsigned char buf[] = "$VNWRG,75,2,80,01,0528*XX\r\n";
  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(sciSendBytes(buf, sizeof(buf), portMAX_DELAY, UART_VN100_REG));
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t stopBinaryOutputs(void) {
  unsigned char buf[] = "$VNWRG,75,0,80,01,0528*XX\r\n";
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
