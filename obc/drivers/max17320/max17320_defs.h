#pragma once

typedef struct {
  uint16_t address;
  uint16_t value;
} configuration_value_map_t;

#define BMS_CONFIGURATION_REGISTER_COUNT 15
typedef enum { REGISTER = 500 } bms_analog_register_t;

enum bms_config_register { REGISTER_CONFIG = 500 };
configuration_value_map_t configurationAddresses[] = {};
