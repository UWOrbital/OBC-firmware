#pragma once

#include "obc_errors.h"
#include "max17320_defs.h"

#include <stdint.h>

obc_error_code_t readBmsRegister(bms_register_t address, uint16_t* data);
obc_error_code_t initBmsInterface();
