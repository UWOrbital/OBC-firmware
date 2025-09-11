#pragma once

#include "obc_errors.h"

#include <stdint.h>
#include <stdbool.h>

#define BMS_MAX_CONFIGURATION_REGISTERS 12U

typedef enum {
  BMS_STATUS = 0x0000,
  BMS_REP_CAP = 0x0005,
  BMS_REP_SOC = 0x0006,
  BMS_VEMPTY = 0x003A
} bms_register_addr_t;

typedef struct {
  uint8_t upperThreshold;
  uint8_t lowerThreshold;
} bms_threshold_value_t;

typedef struct {
  bool isThreshold;
  bms_register_addr_t address;
  union {
    bms_threshold_value_t threshold;
    uint16_t configurationValue;
  };
} bms_register_t;

typedef struct {
  bms_register_t configuration[BMS_MAX_CONFIGURATION_REGISTERS];
} max17320_config_t;

/**
 * @brief Read BMS register value at addresss.
 * @param data Struct value where the the value will be written to. The address is taken from the data->address
 * parameter.
 * @return Error code. OBC_ERR_CODE_SUCCESS if successful.
 */
obc_error_code_t readBmsRegister(bms_register_t* data);

/**
 * @brief Write BMS register value at addresss.
 * @param data Struct which the contains the value that will be written.
 * @return Error code. OBC_ERR_CODE_SUCCESS if successful.
 */
obc_error_code_t writeBmsRegister(bms_register_t* data);

/**
 * @brief Initalizes the BMS interface including all volatile/non-volatile registers
 * that need to be programmed.
 * @param config Struct of type max17320_config_t that contains the configuration data to be programmed.
 * @return Error code.
 * OBC_ERR_CODE_SUCCESS if successful.
 * OBC_ERR_CODE_INVALID_ARG if an invalid argument was given in the configuration.
 */
obc_error_code_t initBmsInterface(max17320_config_t* config);
