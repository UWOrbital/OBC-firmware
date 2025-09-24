#pragma once

#include "obc_errors.h"
#include <stdint.h>
#include <stdbool.h>

#define BMS_NV_CONFIGURATION_REGISTER_COUNT 15u
#define BMS_VOL_CONFIGURATION_REGISTER_COUNT 15u
#define BMS_THRESHOLD_CONFIGURATION_REGISTER_COUNT 15u

/**
 * @struct Key-Value pair struct for programmable configuration register values
 * @param address Internal Address of the register. Must be within bms_register_t enum.
 * @param value The value to which the register needs to be programmed.
 */
typedef struct {
  uint16_t address;
  uint16_t value;
} configuration_value_map_t;

/**
 * @struct Key-Value pair struct for Programmable Measurement Threshold Values
 * @param address Internal Address of the threshold register. Must be within bms_register_t enum.
 * @param lowerTh The lower measurement threshold programmed value.
 * @param upperTh The upper measurement threshold programmed value.
 */
typedef struct {
  uint16_t address;
  uint8_t lowerTh;
  uint8_t upperTh;
} threshold_config_t;

/**
 * @struct Configuration struct for MAX17320 Battery Management IC
 * @param nonVolatileConfiguration Pointer to an array contatining the Non Volatile Addresses/Default Values Pair.
 * @param volatileConfiguration Pointer to an array contatining the volatile Addresses/Default Values Pair.
 * @param measurementThresholds Pointer to an array contatining the measurement threshold values for temperature/voltage
 * etc.
 * @param nonVolatileConfigSize Size of the nonVolatileConfiguration array. Must be at most
 * BMS_NV_CONFIGURATION_REGISTER_COUNT.
 * @param volatileConfigSize Size of the volatileConfiguration array.Must be at most
 * BMS_VOL_CONFIGURATION_REGISTER_COUNT.
 * @param thresholdConfigSize  Size of the measurement thresholds array. Must be at most
 * BMS_THRESHOLD_CONFIGURATION_REGISTER_COUNT
 * @param thresholdIsNonVolatile A bit indicating if the threshold registers are volatile or non-volatile memory.
 */
typedef struct {
  configuration_value_map_t* nonVolatileConfiguration;
  configuration_value_map_t* volatileConfiguration;
  threshold_config_t* measurementThresholds;
  uint8_t nonVolatileConfigSize;
  uint8_t volatileConfigSize;
  uint8_t thresholdConfigSize;
  bool thresholdIsNonVolatile;
} max17320_config_t;

/**
 * @enum List of registers that need to be programmed alongside
 * their corresponding address.
 */
typedef enum { Status = 0x0000, RepCap = 0x0005, RepSOC = 0x0006 } bms_register_t;

typedef enum { COPY_NV, NV_RECALL, WRITE_RESET, FIRMWARE_RESET, AVAILABLE_UPDATES_CHECK } nonvolatile_cmd_t;
