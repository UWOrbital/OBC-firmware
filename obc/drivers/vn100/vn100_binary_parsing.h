#pragma once

#include "obc_errors.h"

#include <stdint.h>
#include <stdlib.h>

/* -------------------------------------- Packet structure Byte Sizes -------------------------- */
#define BINARY_HEADER_SIZE 4U
#define BINARY_CRC_SIZE 2U
#define BINARY_PAYLOAD_SIZE 56U                                                        // Size excluding header and CRC
#define BINARY_PACKET_SIZE BINARY_CRC_SIZE + BINARY_PAYLOAD_SIZE + BINARY_HEADER_SIZE  // Total Size

typedef struct {
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

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Parse the packets into their respective packet types
 * @param packet Unparsed packet in the form of a string
 * @param parsedPacket Buffer with the correct packet type
 *
 * @return OBC_ERR_CODE_SUCCESS on success, else an error code
 */
obc_error_code_t parsePacket(const unsigned char* packet, vn100_binary_packet_t* parsedPacket);

#ifdef __cplusplus
}
#endif
