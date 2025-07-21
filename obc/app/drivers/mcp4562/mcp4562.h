#pragma once

#include "obc_errors.h"

typedef enum {
    VOLATILE_WIPER_0 = 0x0,
    VOLATILE_WIPER_1 = 0x1
} mcp4562_wiper_t;

/**
 * @brief Allows mcp4562 to write data
 *
 * @param address   4 bit device memory address
 * @param data   9 bit data value to write
 *
 * @return obc_error_code_t (success or failure)
 */
obc_error_code_t mcp4562WriteData(uint8_t address, uint16_t data);

/**
 * @brief Allows mcp4562 to write data continuously
 *
 * @param address   4 bit device memory address
 * @param data   16 bit data array (every 2 bits contain 9 bits)
 * @param length    length of data array
 *
 * @return obc_error_code_t (success or failure)
 */
obc_error_code_t mcp4562WriteContData(uint8_t address, uint16_t data[], size_t length);

/**
 * @brief Allows mcp4562 to read data
 *
 * @param data   Pointer to 16 bit variable to store 9 bit data
 *
 * @return obc_error_code_t (success or failure)
 */
obc_error_code_t mcp4562ReadData(uint16_t *data);

/**
 * @brief Allows mcp4562 to randomly read data sent
 *
 * @param address   4 bit device memory address
 * @param data   Pointer to 16 bit variable to store 9 bit data
 *
 * @return obc_error_code_t (success or failure)
 */
obc_error_code_t mcp4562ReadRandomData(uint8_t address, uint16_t *data);

/**
 * @brief Allows mcp4562 to read data continuously
 *
 * @param data   16 bit data array (every entry contain 9 bits)
 * @param length    length of data array
 *
 * @return obc_error_code_t (success or failure)
 */
obc_error_code_t mcp4562ReadContData(uint16_t data[], size_t length);

/**
 * @brief allows mcp4562 to increment wiper from n to n+1 (change resistance)
 *
 * @param address   4 bit device memory address
 *
 * @return obc_error_code_t (success or failure)
 */
obc_error_code_t mcp4562IncrementWiper(uint8_t address);

/**
 * @brief allows mcp4562 to decrement wiper from n to n-1 (change resistance)
 *
 * @param address   4 bit device memory address
 *
 * @return obc_error_code_t (success or failure)
 */
obc_error_code_t mcp4562DecrementWiper(uint8_t address);