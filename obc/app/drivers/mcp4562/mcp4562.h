#pragma once

#include "obc_errors.h"

#include <stdint.h>

// Driver implementation for the MCP4562 Device
// Only volatile commands are implemented

// MCP45X2 I2C Device Address; page 50 in datasheet;  ‘0101 1’b + A1:A0
// A1:A0 is variable, so I just chose '00' for now.
#define MCP4562_OBC_I2C_ADDRESS 0x2CU

// not sure about this atm -> ask in discord how to determine these numbers
#define I2C_MUTEX_TIMEOUT portMAX_DELAY
#define I2C_TRANSFER_TIMEOUT pdMS_TO_TICKS(100)

// I2C command operation bits
#define I2C_WRITE 0x0U
#define I2C_READ 0x3U
#define I2C_INCREMENT_WIPER 0x1U
#define I2C_DECREMENT_WIPER 0x2U

// Wiper addresses
#define V_WIPER_0_ADDR 0x0U
#define V_WIPER_1_ADDR 0x1U
#define NV_WIPER_0_ADDR 0x2U
#define NV_WIPER_1_ADDR 0x3U

// TCON register address
#define V_TCON_ADDR 0x04U

// Full and zero scale wiper values
#define MAX_WIPER_POS 0x100U
#define MIN_WIPER_POS 0x0U

/**
 * @brief Increment wiper by 1
 *
 * @param wiper One of two wipers: 0 or 1
 * @return OBC_ERR_CODE_SUCCESS if successful, error code otherwise
 */
obc_error_code_t incWiper(int wiper);

/**
 * @brief Decrement wiper by 1
 *
 * @param wiper One of two wipers: 0 or 1
 * @return OBC_ERR_CODE_SUCCESS if successful, error code otherwise
 */
obc_error_code_t decWiper(int wiper);

/**
 * @brief Set wiper position
 *
 * @param pos position of wiper
 * @param wiper one of two wipers: 0 or 1
 * @return OBC_ERR_CODE_SUCCESS if successful, error code otherwise
 */
obc_error_code_t writeWiperPosition(uint16_t pos, int wiper);

/**
 * @brief Read wiper position
 *
 * @param pos pointer to store wiper position
 * @param wiper one of two wipers: 0 or 1
 * @return OBC_ERR_CODE_SUCCESS if successful, error code otherwise
 */
obc_error_code_t readWiperPosition(uint16_t* pos, int wiper);
