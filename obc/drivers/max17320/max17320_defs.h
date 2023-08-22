#pragma once

#include "obc_errors.h"
#include <stdint.h>

#define BMS_NV_CONFIGURATION_REGISTER_COUNT 15
#define BMS_VOL_CONFIGURATION_REGISTER_COUNT 15

typedef struct {
  uint16_t address;
  uint16_t value;
} configuration_value_map_t;

typedef struct {
  uint16_t address;
  uint8_t lowerTh;
  uint8_t upperTh;
} threshold_config_t;

typedef enum { Status = 0x0000, RepCap = 0x0005, RepSOC = 0x0006 } bms_register_t;

configuration_value_map_t nonVolatileConfiguration[] = {};
threshold_config_t analogThresholds[] = {};
configuration_value_map_t volatileConfiguration[] = {};
