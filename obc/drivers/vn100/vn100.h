#pragma once

#include "obc_errors.h"
#include <stdint.h>

typedef enum { 
    VN_YPR,         // Get yaw, pitch and roll measurements 
    VN_MAG,         // Get magnetic measurements
    VN_ACC,         // Get acceleration measurements
    VN_GYR,         // Get angular rate measurements
    VN_YMR          // Get all of the above
} vn_cmd_t;

typedef struct {
    double yaw;
    double pitch;
    double roll;
} vn_ypr_packet_t; 

typedef struct {
    double magnet;
} vn_mag_packet_t;

typedef struct {
    double accel;
} vn_accel_packet_t;

typedef struct {
    double gyro;
} vn_gyro_packet_t;

typedef struct {
    double yaw;
    double pitch;
    double roll;
    double magnet;
    double accel;
    double gyro;
} vn_all_packet_t;

/**
 * @brief Initiates VN100 with the default parameters for baudrate, polling rate, etc
 *
 */
void initVN100(void);

obc_error_code_t resetModule();

/**
 * @brief Set the baudrate for the VN-100 peripheral
 * @param baudrate The desired baudrate to be set
 *
 * @return OBC_ERR_CODE_SUCCESS on success, else an error code
 */
obc_error_code_t VN100SetBaudrate(uint32_t baudrate);

/**
 * @brief Sends a request to the VN-100 via UART to return a packet containing Yaw, Pitch and Roll
 *
 * @return OBC_ERR_CODE_SUCCESS on success, else an error code
 */
obc_error_code_t requestYPR();

/**
 * @brief Retrieve the Yaw, Pitch and Roll from the VN-100
 *
 * @param packet Packet to store the returned information in
 * @return OBC_ERR_CODE_SUCCESS on success, else an error code
 */
obc_error_code_t retrieveYPR(vn_ypr_packet_t *packet);