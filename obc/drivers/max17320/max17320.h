#pragma once

#include "obc_errors.h"
#include "max17320_defs.h"

#include <stdint.h>

/**
 * @brief Read BMS register value at addresss.
 * @param address The value of the address to be read of type bms_register_t.
 * @param data uint16_t value where the the value will be written to.
 * @return Error code. OBC_ERR_CODE_SUCCESS if successful.
 */
obc_error_code_t readBmsRegister(bms_register_t address, uint16_t* data);

/**
 * @brief Initalizes the BMS interface including all volatile/non-volatile registers
 * that need to be programmed.
 * @param config Struct of type max17320_config_t that contains the configuration data to be programmed.
 * @return Error code.
 * OBC_ERR_CODE_SUCCESS if successful.
 * OBC_ERR_CODE_BMS_REACHED_MAXIMUM_CONFIG_UPDATES if the nonvolatile memory cannot
 * be updated anynore.
 * OBC_ERR_CODE_BMS_REACHED_MAXIMUM_COUNT if after maximum repeated tries, the non-volatile memory
 * cannot be programmed, indicated by a status bit not clearning in the IC.
 * OBC_ERR_CODE_INVALID_ARG if an invalid argument was given in the configuration.
 */
obc_error_code_t initBmsInterface(max17320_config_t config);
