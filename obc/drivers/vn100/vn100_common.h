#pragma once

#include "obc_errors.h"
#include "vn100_ascii.h"
#include "vn100_binary.h"
#include "vn100_parsing.h"

#include <stdint.h>

/* To access the user manual for VN-100 click here --> https://geo-matching.com/media/migrationpiwnum.pdf */

/**
 * @brief Initiates VN100 with the default parameters for baudrate, polling rate, etc
 * Defaults:
 * User Tag: NULL
 * Serial Baud Rate: 115200
 * Async Data Output Frequency: 10 Hz
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
 * @brief pause asyncronous outputs from the VN-100
 *
 * @return OBC_ERR_CODE_SUCCESS on success, else an error code
 */
obc_error_code_t pauseASYNC();

/**
 * @brief resume asyncronous outputs from the VN-100
 *
 * @return OBC_ERR_CODE_SUCCESS on success, else an error code
 */
obc_error_code_t resumeASYNC();
