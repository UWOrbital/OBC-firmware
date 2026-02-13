#include <stdint.h>
#include <stdio.h>
#include "obc_errors.h"
#include "os_portmacro.h"

static uint8_t mockData[2] = {0, 0};

void i2cSetSlaveAdd(void* i2c, unsigned int slaveAdd) {}
void i2cSetDirection(void* i2c, unsigned int direction) {}
void i2cSetCount(void* i2c, unsigned int count) {}
void i2cSetMode(void* i2c, unsigned int mode) {}
void i2cSetStop(void* i2c) {}
void i2cSetStart(void* i2c) {}
void i2cSend(void* i2c, unsigned char* data, unsigned int length) {}
void i2cReceive(void* i2c, unsigned char* data, unsigned int length) {}
void i2cClearSCD(void* i2c) {}

obc_error_code_t i2cWriteReg(uint8_t sAddr, uint8_t reg, uint8_t* data, uint16_t numBytes) {
  // Simulate a successful write
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t i2cReadReg(uint8_t sAddr, uint8_t reg, uint8_t* data, uint16_t numBytes,
                            TickType_t transferTimeoutTicks) {
  if ((sAddr == 0b1000000U || sAddr == 0b1000001U) && numBytes == 2) {
    // 0x01 - shunt voltage
    // 0x02 - bus voltage
    // 0x03 - power
    // 0x04 - current
    if (reg == 0x01 || reg == 0x02 || reg == 0x03 || reg == 0x04) {  // Bus voltage register
      data[0] = mockData[0];                                         // High byte
      data[1] = mockData[1];                                         // Low byte
    }
    return OBC_ERR_CODE_SUCCESS;
  }
  // Default mock behavior
  return OBC_ERR_CODE_SUCCESS;
}

void setMockBusVoltageValue(float expectedVoltage) {
  uint16_t expectedVal = (uint16_t)((expectedVoltage / 0.00125f) + 0.5f);
  mockData[0] = (0xFF00 & expectedVal) >> 8;  // High byte
  mockData[1] = 0xFF & expectedVal;           // Low bytes
}

void setMockCurrentValue(float expectedCurrent) {
  uint16_t expectedVal = expectedCurrent / 0.001f;
  mockData[0] = (0xFF00 & expectedVal) >> 8;  // High byte
  mockData[1] = 0xFF & expectedVal;           // Low bytes
}

void setMockPowerValue(float expectedPower) {
  // add 0.5 to round to the nearest integer (ensures that .5-.9 rounds up instead of truncating the decimal)
  uint16_t expectedVal = (uint16_t)((expectedPower / 0.025f) + 0.5f);
  mockData[0] = (0xFF00 & expectedVal) >> 8;  // High byte
  mockData[1] = 0xFF & expectedVal;           // Low bytes
}

void setMockShuntVoltageValue(float expectedShuntVoltage) {
  // if expected value is positive, add 0.5; if negative, subtract 0.5
  int32_t raw = (int32_t)((expectedShuntVoltage / 0.0000025f) + (expectedShuntVoltage >= 0 ? 0.5f : -0.5f));
  uint16_t expectedVal = (uint16_t)raw;     // two's complement encoding for negative values
  mockData[0] = (expectedVal >> 8) & 0xFF;  // High byte
  mockData[1] = expectedVal & 0xFF;         // Low byte
}
