#pragma once

#include "obc_errors.h"
#include <stdint.h>

typedef enum {
  VN_YPR,  // Get yaw, pitch and roll measurements
  VN_MAG,  // Get magnetic measurements
  VN_ACC,  // Get acceleration measurements
  VN_GYR,  // Get angular rate measurements
  VN_YMR   // Get all of the above
} vn_cmd_t;

typedef struct {
  float yaw;
  float pitch;
  float roll;
} vn_ypr_packet_t;

typedef struct {
  float magnet;
} vn_mag_packet_t;

typedef struct {
  float accel;
} vn_accel_packet_t;

typedef struct {
  float gyro;
} vn_gyro_packet_t;

typedef struct {
  float yaw;
  float pitch;
  float roll;
  float magnet;
  float accel;
  float gyro;
} vn_ymr_packet_t;

/**
 * @brief Initiates VN100 with the default parameters for baudrate, polling rate, etc
 * Factory Defaults:
 * User Tag: NULL
 * Serial Baud Rate: 115200
 * Async Data Output Frequency: 40 Hz
 * Async Data Output Type: INS_LLA
 * Syncronization control: 3,0,0,0,6,1,0,100000000,0
 *
 */
void initVN100(void);

obc_error_code_t resetModule();

/**
 * @brief Set the baudrate for the VN-100 peripheral
 * @param baudrate The desired baudrate to be set
 * @note The default baudrate is 115200 Hz
 *
 * @return OBC_ERR_CODE_SUCCESS on success, else an error code
 */
obc_error_code_t VN100SetBaudrate(uint32_t baudrate);

/**
 * @brief Retrieve the Yaw, Pitch and Roll from the VN-100
 *
 * @param packet Packet to store the returned information in
 * @return OBC_ERR_CODE_SUCCESS on success, else an error code
 */
obc_error_code_t retrieveYPR(vn_ypr_packet_t *packet);

/**
 * @brief Retrieve the Yaw, Pitch and Roll from the VN-100
 *
 * @param packet Packet to store the returned information in
 * @return OBC_ERR_CODE_SUCCESS on success, else an error code
 */
obc_error_code_t retrieveMAG(vn_mag_packet_t *packet);

/**
 * @brief Retrieve the Yaw, Pitch and Roll from the VN-100
 *
 * @param packet Packet to store the returned information in
 * @return OBC_ERR_CODE_SUCCESS on success, else an error code
 */
obc_error_code_t retrieveACCEL(vn_accel_packet_t *packet);

/**
 * @brief Retrieve the Yaw, Pitch and Roll from the VN-100
 *
 * @param packet Packet to store the returned information in
 * @return OBC_ERR_CODE_SUCCESS on success, else an error code
 */
obc_error_code_t retrieveGYRO(vn_gyro_packet_t *packet);

/**
 * @brief Retrieve the Yaw, Pitch and Roll, Magnetometer, Acceleration, from the VN-100
 *
 * @param packet Packet to store the returned information in
 * @return OBC_ERR_CODE_SUCCESS on success, else an error code
 */
obc_error_code_t retrieveYMR(vn_ymr_packet_t *packet);

/**
 * @brief Configure which packet type will be asyncronously outputted, note that this will also change the packet header
 * type
 *
 * @param cmd Command to denote which packet type to use
 * @return OBC_ERR_CODE_SUCCESS on success, else an error code
 */
obc_error_code_t setASYNCOutputs(vn_cmd_t cmd);
