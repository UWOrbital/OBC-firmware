#pragma once

#include "obc_errors.h"

typedef enum { INA230_DEVICE_ONE = 0x00, INA230_DEVICE_TWO, INA230_DEVICE_COUNT } ina230_device_t;

obc_error_code_t initINA230();
obc_error_code_t readAndDisableIfAlert(ina230_device_t device);
