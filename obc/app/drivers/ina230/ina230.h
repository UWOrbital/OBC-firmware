#pragma once

#include "obc_errors.h"
#include <stdint.h>

typedef enum { INA230_DEVICE_ONE = 0x00, INA230_DEVICE_TWO, INA230_DEVICE_COUNT } ina230_device_t;

obc_error_code_t initINA230();
obc_error_code_t readAndDisableIfAlert(ina230_device_t device);
obc_error_code_t getINA230ShuntVoltage(uint8_t i2cAddress, float* shuntVoltage);
obc_error_code_t getINA230ShuntVoltageForDevice(uint8_t deviceIndex, float* shuntVoltage);
void main_usage();
