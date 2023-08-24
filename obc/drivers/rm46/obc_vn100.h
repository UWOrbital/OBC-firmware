#pragma once

#include "obc_errors.h"
#include <stdint.h>

#define VN100_UART_REG  UART_PRINT_REG
#define VN100_BAUDRATE  152000 

enum packetTypes { 
    
};

struct ypr_packet {
    double yaw;
    double pitch;
    double roll;
};

/**
 * @brief Initiates VN100 with the necessary
 *
 * @return OBC_ERR_CODE_SUCCESS on success, else an error code
 */
void initVN100(void);

/**
 * @brief Set the baudrate for the VN-100 peripheral
 * @param baudrate The desired baudrate to be set
 *
 * @return OBC_ERR_CODE_SUCCESS on success, else an error code
 */
obc_error_code_t VN100SetBaudRate(int baudrate);

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
obc_error_code_t retrieveYPR(ypr_packet* packet);