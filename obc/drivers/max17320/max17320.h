#pragma once

#include "obc_errors.h"
#include "max17320_defs.h"

#include <stdint.h>

obc_error_code_t writeAnalogRegister(bms_analog_register_t register, uint16_t data);
obc_error_code_t readAnalogRegister(bms_analog_register_t register, uint16_t data);

obc_error_code_t initalizeConfigurationRegisters();
obc_error_code_t readConfigurationRegister(bms_config_register_t register, uint16_t data);
