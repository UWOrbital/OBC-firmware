#include <stdint.h>
#include "obc_errors.h"
#include "os_portmacro.h"

void i2cSetSlaveAdd(void* i2c, unsigned int slaveAdd) {}
void i2cSetDirection(void* i2c, unsigned int direction) {}
void i2cSetCount(void* i2c, unsigned int count) {}
void i2cSetMode(void* i2c, unsigned int mode) {}
void i2cSetStop(void* i2c) {}
void i2cSetStart(void* i2c) {}
void i2cSend(void* i2c, unsigned char* data, unsigned int length) {}
void i2cReceive(void* i2c, unsigned char* data, unsigned int length) {}
void i2cClearSCD(void* i2c) {}

obc_error_code_t i2cWriteReg(uint8_t sAddr, uint8_t reg, uint8_t *data, uint16_t numBytes) {
    // Simulate a successful write
    return OBC_ERR_CODE_SUCCESS;
}


obc_error_code_t i2cReadReg(uint8_t sAddr, uint8_t reg, uint8_t *data, uint16_t numBytes, TickType_t transferTimeoutTicks) {
    if ((sAddr == 0b1000000U || sAddr == 0b1000001U) && reg == 0x02 && numBytes == 2) { // Bus voltage register
        data[0] = 0x10; // High byte
        data[1] = 0x00; // Low byte
        return OBC_ERR_CODE_SUCCESS;
    }
    // Default mock behavior
    return OBC_ERR_CODE_SUCCESS;
}