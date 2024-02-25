#pragma once

#include "obc_errors.h"

typedef enum ina230_device { INA230_ONE, INA230_TWO } ina230_device_t;

obc_error_code_t initINA230Interface();
obc_error_code_t readAndDisableIfAlert(ina230_device_t device);
