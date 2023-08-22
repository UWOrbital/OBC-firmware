#pragma once

#include "obc_errors.h"
#include "max17320_defs.h"

#include <stdint.h>

obc_error_code_t readAnalogValue(bms_analog_register_t address, uint16_t data);
obc_error_code_t initBmsInterface();
obc_error_code_t readConfigurationRegister(uint16_t address, uint16_t* returnData);
