#pragma once

#include "obc_errors.h"

#include <stdint.h>

#define BMS_INTERNAL_MEM_LOWER_0 0x000
#define BMS_INTERNAL_MEM_UPPER_0 0x0FF
#define BMS_INTERNAL_MEM_LOWER_1 0x180
#define BMS_INTERNAL_MEM_UPPER_1 0x1FF

#define BMS_INTERNAL_MEM_MASK 0x00FF
#define BMS_SLAVE_BIT_MASK (1 << 8)

#define BMS_I2C_SLAVE_ADDR_0 15
#define BMS_I2C_SLAVE_ADDR_1 20
