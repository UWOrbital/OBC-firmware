#pragma once

#include "obc_errors.h"

/**
 * @brief Write a 9-bit value to a specified register address
 *
 * @return obc_error_code_t - whether or not the write was successful
 */
obc_error_code_t mcp4562WriteData(uint8_t address, uint16_t data);

/**
 * @brief Write multiple 9-bit values to the same register continuously
 *
 * @return obc_error_code_t - whether or not the continuous write was successful
 */
obc_error_code_t mcp4562WriteContData(uint8_t address, uint16_t data[], size_t length);

/**
 * @brief Read the most recently addressed register
 *
 * @return obc_error_code_t - whether or not the read was successful
 */
obc_error_code_t mcp4562ReadData(uint16_t *data);

/**
 * @brief Random read from a specific register
 *
 * @return obc_error_code_t - whether or not the random read was successful
 */
obc_error_code_t mcp4562ReadRandomData(uint8_t address, uint16_t *data);

/**
 * @brief Continuously read multiple 9-bit values from sequential memory locations
 *
 * @return obc_error_code_t - whether or not the continuous read was successful
 */
obc_error_code_t mcp4562ReadContData(uint16_t data[], size_t length);

/**
 * @brief Increment the wiper register value by 1
 * 
 * @return obc_error_code_t - whether or not the increment was successful
 */
obc_error_code_t mcp4562IncrementWiper(uint8_t address);

/**
 * @brief Decrement the wiper register value by 1
 *
 * @return obc_error_code_t - whether or not the decrement was successful
 */
obc_error_code_t mcp4562DecrementWiper(uint8_t address);