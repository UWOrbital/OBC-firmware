#include "mcp4562.h"
#include "obc_errors.h"
#include "obc_logging.h"
#include "obc_i2c_io.h"

#include <stdint.h>

#define MCP_ADDRESS 0x50 // placeholder idk
#define MCP_BYTE_SIZE_2 0x2
#define MCP_BYTE_SIZE_1 0x1
#define MCP_MAX_DATA_SIZE 0x1FF
#define MCP_WRITE   0x0
#define MCP_READ   0x3
#define MCP_INCREMENT   0x1
#define MCP_DECREMENT   0x2
#define VOLATILE_WIPER_0    0x0
#define VOLATILE_WIPER_1    0x1
#define VOLATILE_TCON   0x4

#define I2C_MUTEX_TIMEOUT portMAX_DELAY
#define I2C_TRANSFER_TIMEOUT pdMS_TO_TICKS(100)

/**
 * @brief Allows mcp4562 to write data
 *
 * @param address   4 bit device memory address
 * @param data   9 bit data value to write
 *
 * @return obc_error_code_t (success or failure)
 */
obc_error_code_t mcp4562WriteData(uint8_t address, uint16_t data) {
    if(data > MCP_MAX_DATA_SIZE || data == NULL)
        return OBC_ERR_CODE_INVALID_ARG;

    uint8_t buffer[2];

    buffer[0] = (address << 4) | (MCP_WRITE << 2) | ((0x200 & data) >> 9);
    buffer[1] = data;
    
    RETURN_IF_ERROR_CODE(
        i2cSendTo(MCP_ADDRESS, MCP_BYTE_SIZE_2, &buffer, I2C_MUTEX_TIMEOUT, I2C_TRANSFER_TIMEOUT));
    return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief Allows mcp4562 to write data continuously
 *
 * @param address   4 bit device memory address
 * @param data   16 bit data array (every 2 bits contain 9 bits)
 * @param length    length of data array
 *
 * @return obc_error_code_t (success or failure)
 */
obc_error_code_t mcp4562WriteContData(uint8_t address, uint16_t data[], size_t length) {
    if(length == 0 || (address != VOLATILE_WIPER_0 && address != VOLATILE_WIPER_1 && address != VOLATILE_TCON))
        return OBC_ERR_CODE_INVALID_ARG;

    uint8_t buffer[length*2];
    size_t index = 0;

    for(size_t i = 0; i < length; i++){
        buffer[index] = (address << 4) | (MCP_WRITE << 2) | ((0x200 & data[i]) >> 9);
        buffer[index+1] = data[i];
        index += 2;
    }
    
    RETURN_IF_ERROR_CODE(
        i2cSendTo(MCP_ADDRESS, length*2, buffer, I2C_MUTEX_TIMEOUT, I2C_TRANSFER_TIMEOUT));
    return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief Allows mcp4562 to read data sent
 *
 * @param data   Pointer to 16 bit variable to store 9 bit data
 *
 * @return obc_error_code_t (success or failure)
 */
obc_error_code_t mcp4562ReadData(uint16_t *data) {
    if(data == NULL)
        return OBC_ERR_CODE_INVALID_ARG;

    uint8_t buffer[2];
    
    RETURN_IF_ERROR_CODE(
        i2cReceiveFrom(MCP_ADDRESS | 0x1, MCP_BYTE_SIZE_2, &buffer, I2C_MUTEX_TIMEOUT, I2C_TRANSFER_TIMEOUT));

    *data = (buffer[0] << 8) | buffer[1];

    return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief Allows mcp4562 to randomly read data sent
 *
 * @param address   4 bit device memory address
 * @param data   Pointer to 16 bit variable to store 9 bit data
 *
 * @return obc_error_code_t (success or failure)
 */
obc_error_code_t mcp4562ReadRandomData(uint8_t address, uint16_t *data) {
    if(data == NULL)
        return OBC_ERR_CODE_INVALID_ARG;

    uint8_t command = (address << 4) | (MCP_READ << 2);

    RETURN_IF_ERROR_CODE(
        i2cSendTo(MCP_ADDRESS, MCP_BYTE_SIZE_1, &command, I2C_MUTEX_TIMEOUT, I2C_TRANSFER_TIMEOUT));

    uint8_t buffer[2];
    
    RETURN_IF_ERROR_CODE(
        i2cReceiveFrom(MCP_ADDRESS | 0x1, MCP_BYTE_SIZE_2, &buffer, I2C_MUTEX_TIMEOUT, I2C_TRANSFER_TIMEOUT));

    *data = (buffer[0] << 8) | buffer[1];

    return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief Allows mcp4562 to read data continuously
 *
 * @param data   16 bit data array (every entry contain 9 bits)
 * @param length    length of data array
 *
 * @return obc_error_code_t (success or failure)
 */
obc_error_code_t mcp4562ReadContData(uint16_t data[], size_t length) {
    if(length == 0)
        return OBC_ERR_CODE_INVALID_ARG;

    uint8_t buffer[length*2];
    size_t index = 0;
    
    RETURN_IF_ERROR_CODE(
        i2cReceiveFrom(MCP_ADDRESS | 0x1, length*2, buffer, I2C_MUTEX_TIMEOUT, I2C_TRANSFER_TIMEOUT));

    for(size_t i = 0; i < length*2; i+=2){
        data[index] = (buffer[i] << 8) | buffer[i+1];
        index++;
    }

    return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief allows mcp4562 to increment wiper from n to n+1 (change resistance)
 *
 * @param address   4 bit device memory address
 *
 * @return obc_error_code_t (success or failure)
 */
obc_error_code_t mcp4562IncrementWiper(uint8_t address) {
    if(address != VOLATILE_WIPER_0 && address != VOLATILE_WIPER_1)
        return OBC_ERR_CODE_INVALID_ARG;

    uint8_t buffer;

    buffer = (address << 4) | (MCP_INCREMENT << 2);
    
    RETURN_IF_ERROR_CODE(
        i2cSendTo(MCP_ADDRESS, MCP_BYTE_SIZE_1, &buffer, I2C_MUTEX_TIMEOUT, I2C_TRANSFER_TIMEOUT));
    return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief allows mcp4562 to decrement wiper from n to n-1 (change resistance)
 *
 * @param address   4 bit device memory address
 *
 * @return obc_error_code_t (success or failure)
 */
obc_error_code_t mcp4562DecrementWiper(uint8_t address) {
    if(address != VOLATILE_WIPER_0 && address != VOLATILE_WIPER_1)
        return OBC_ERR_CODE_INVALID_ARG;

    uint8_t buffer;

    buffer = (address << 4) | (MCP_INCREMENT << 2);
    
    RETURN_IF_ERROR_CODE(
        i2cSendTo(MCP_ADDRESS, MCP_BYTE_SIZE_1, &buffer, I2C_MUTEX_TIMEOUT, I2C_TRANSFER_TIMEOUT));
    return OBC_ERR_CODE_SUCCESS;
}