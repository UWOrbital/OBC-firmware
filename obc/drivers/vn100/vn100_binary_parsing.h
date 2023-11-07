#pragma once

#include "obc_errors.h"

#include <stdint.h>
#include <stdlib.h>

/* -------------------------------------- Packet structure Byte Sizes -------------------------- */
#define BINARY_HEADER_SIZE 4U
#define BINARY_CRC_SIZE 2U
#define BINARY_PAYLOAD_SIZE 56U                                                        // Size excluding header and CRC
#define BINARY_PACKET_SIZE BINARY_CRC_SIZE + BINARY_PAYLOAD_SIZE + BINARY_HEADER_SIZE  // Total Size

typedef struct __attribute__((__packed__)) {
  float yaw;
  float pitch;
  float roll;
  float gyroX;
  float gyroY;
  float gyroZ;
  float accelX;
  float accelY;
  float accelZ;
  float magX;
  float magY;
  float magZ;
  float temp;
  float pres;
} vn100_binary_packet_t;

typedef enum {
  HARD_FAULT = 1,
  SERIAL_BUFFER_OVERFLOW = 2,
  INVALID_CHECKSUM = 3,
  INVALID_COMMAND = 4,
  NOT_ENOUGH_PARAMETERS = 5,
  TOO_MANY_PARAMETERS = 6,
  INVALID_PARAMETER = 7,
  INVALID_REGISTER = 8,
  UNATHORIZED_ACCESS = 9,
  WATCHDOG_RESET = 10,
  OUTPUT_BUFFER_OVERFLOW = 11,
  INSUFFICIENT_BAUD_RATE = 12,
  ERROR_BUFFER_OVERFLOW = 255
} vn100_error_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Parse the packets into their respective packet types
 * @param cmd Command that denotes which packet type it is
 * @param packet Unparsed packet in the form of a string
 * @param parsedPacket Buffer with the correct packet type
 * @param error Error code in case an error occurs
 *
 * @return OBC_ERR_CODE_SUCCESS on success, else an error code
 */
obc_error_code_t parsePacket(unsigned char* packet, vn100_binary_packet_t* parsedPacket, vn100_error_t* error);

#ifdef __cplusplus
}
#endif
