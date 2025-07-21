#include "mcp4562.h"
#include "obc_errors.h"
#include "obc_logging.h"
#include "obc_i2c_io.h"

#include <stdint.h>

#define MCP_ADDRESS 0x50 // placeholder idk
#define MCP_MAX_DATA_SIZE 0x1FF
#define MCP_WRITE   0x0
#define MCP_READ   0x3
#define MCP_INCREMENT   0x1
#define MCP_DECREMENT   0x2
#define VOLATILE_TCON   0x4

#define I2C_MUTEX_TIMEOUT portMAX_DELAY
#define I2C_TRANSFER_TIMEOUT pdMS_TO_TICKS(100)

obc_error_code_t mcp4562WriteData(uint8_t address, uint16_t data) {
    if(data > MCP_MAX_DATA_SIZE || data == 0)
        return OBC_ERR_CODE_SIZE_OVERFLOW;

    uint8_t buffer[2];

    buffer[0] = (address << 4) | (MCP_WRITE << 2) | ((0x200 & data) >> 9);
    buffer[1] = data;
    
    RETURN_IF_ERROR_CODE(
        i2cSendTo(MCP_ADDRESS, sizeof(buffer), &buffer, I2C_MUTEX_TIMEOUT, I2C_TRANSFER_TIMEOUT));
    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t mcp4562WriteContData(uint8_t address, uint16_t data[], size_t length) {
    if(length == 0 || (address != VOLATILE_WIPER_0 && address != VOLATILE_WIPER_1 && address != VOLATILE_TCON))
        return OBC_ERR_CODE_INVALID_ARG;

    uint8_t buffer[length*2];
    size_t index = 0;

    for(size_t i = 0; i < length; i++){
        if(data[i] > MCP_MAX_DATA_SIZE){
            return OBC_ERR_CODE_INVALID_ARG;
        }
        buffer[index] = (address << 4) | (MCP_WRITE << 2) | ((0x200 & data[i]) >> 9);
        buffer[index+1] = data[i];
        index += 2;
    }
    
    RETURN_IF_ERROR_CODE(
        i2cSendTo(MCP_ADDRESS, length*2, buffer, I2C_MUTEX_TIMEOUT, I2C_TRANSFER_TIMEOUT));
    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t mcp4562ReadData(uint16_t *data) {
    if(data == NULL)
        return OBC_ERR_CODE_INVALID_ARG;

    uint8_t buffer[2];
    
    RETURN_IF_ERROR_CODE(
        i2cReceiveFrom(MCP_ADDRESS | 0x1, sizeof(buffer), &buffer, I2C_MUTEX_TIMEOUT, I2C_TRANSFER_TIMEOUT));

    *data = (buffer[0] << 8) | buffer[1];

    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t mcp4562ReadRandomData(uint8_t address, uint16_t *data) {
    if(data == NULL)
        return OBC_ERR_CODE_INVALID_ARG;

    uint8_t command = (address << 4) | (MCP_READ << 2);

    RETURN_IF_ERROR_CODE(
        i2cSendTo(MCP_ADDRESS, sizeof(buffer), &command, I2C_MUTEX_TIMEOUT, I2C_TRANSFER_TIMEOUT));

    uint8_t buffer[2];
    
    RETURN_IF_ERROR_CODE(
        i2cReceiveFrom(MCP_ADDRESS | 0x1, sizeof(buffer), &buffer, I2C_MUTEX_TIMEOUT, I2C_TRANSFER_TIMEOUT));

    *data = (buffer[0] << 8) | buffer[1];

    return OBC_ERR_CODE_SUCCESS;
}

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

obc_error_code_t mcp4562IncrementWiper(uint8_t address) {
    if(address != VOLATILE_WIPER_0 && address != VOLATILE_WIPER_1)
        return OBC_ERR_CODE_INVALID_ARG;

    uint8_t buffer;

    buffer = (address << 4) | (MCP_INCREMENT << 2);
    
    RETURN_IF_ERROR_CODE(
        i2cSendTo(MCP_ADDRESS, sizeof(buffer), &buffer, I2C_MUTEX_TIMEOUT, I2C_TRANSFER_TIMEOUT));
    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t mcp4562DecrementWiper(uint8_t address) {
    if(address != VOLATILE_WIPER_0 && address != VOLATILE_WIPER_1)
        return OBC_ERR_CODE_INVALID_ARG;

    uint8_t buffer;

    buffer = (address << 4) | (MCP_INCREMENT << 2);
    
    RETURN_IF_ERROR_CODE(
        i2cSendTo(MCP_ADDRESS, sizeof(buffer), &buffer, I2C_MUTEX_TIMEOUT, I2C_TRANSFER_TIMEOUT));
    return OBC_ERR_CODE_SUCCESS;
}