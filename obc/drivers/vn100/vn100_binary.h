#pragma once

#include "obc_errors.h"
#include <stdint.h>

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
} vn_binary_packet_t;

/**
 * @brief Function begins asynrconous binary outputs on VN_100 Serial port 2 (TTL)
 *  Outputs:
 *  Yaw, Pitch, Roll
 *  Angular Rates
 *  Acceleration
 *  Magnetometer
 *  Temperature
 *  Pressure
 *
 * @return OBC_ERR_CODE_SUCCESS on success, else an error code
 */
obc_error_code_t startBinaryOutputs(void);

/**
 * @brief Stops asyncronous binary ouputs from serial port 2, this should be used in the event of changing the
 *        configuration of serial port 2 to output serial ASCII. Simply call startBinaryOutputs() to
 *        reconfigure the serial port to output binary packets.
 *
 * @return OBC_ERR_CODE_SUCCESS on success, else an error code
 */
obc_error_code_t stopBinaryOutputs(void);

/**
 * @brief Reads all incoming asyncronous binary output data.
 *
 * @param parsedPacket Pointer to the packet to store data in.
 * @return OBC_ERR_CODE_SUCCESS on success, else an error code
 */
obc_error_code_t readBinaryOutputs(void* parsedPacket);
