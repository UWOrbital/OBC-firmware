#ifdef __cplusplus
extern "C" {
#endif

#pragma once

#include "obc_errors.h"
#include <stdint.h>

#ifdef USE_MOCK_I2C
    #ifndef TICK_TYPE_H
        typedef uint32_t TickType_t;
    #endif
#else
    #include "os_portmacro.h"
#endif

typedef enum { INA230_DEVICE_ONE = 0x00, INA230_DEVICE_TWO, INA230_DEVICE_COUNT } ina230_device_t;

// function pointers to switch between mock and real data
extern obc_error_code_t (*i2cReadRegFuncPtr)(uint8_t, uint8_t, uint8_t*, uint16_t, TickType_t);
extern obc_error_code_t (*i2cWriteRegFuncPtr)(uint8_t, uint8_t, uint8_t*, uint16_t);

obc_error_code_t initINA230();
obc_error_code_t readAndDisableIfAlert(ina230_device_t device);
obc_error_code_t getINA230ShuntVoltage(uint8_t i2cAddress, float* shuntVoltage);
obc_error_code_t getINA230ShuntVoltageForDevice(uint8_t deviceIndex, float* shuntVoltage);
void main_usage();
obc_error_code_t disableNoAlert(ina230_device_t device);
obc_error_code_t getINA230BusVoltage(uint8_t i2cAddress, float* busVoltage);
obc_error_code_t getINA230BusVoltageForDevice(uint8_t deviceIndex, float* busVoltage);
obc_error_code_t getINA230Power(uint8_t i2cAddress, float* power);
obc_error_code_t getINA230PowerForDevice(uint8_t deviceIndex, float* power);
obc_error_code_t getINA230Current(uint8_t i2cAddress, float* power);
obc_error_code_t getINA230CurrentForDevice(uint8_t deviceIndex, float* power);

#ifdef __cplusplus
}
#endif