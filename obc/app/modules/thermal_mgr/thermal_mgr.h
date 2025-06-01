#pragma once

#include "obc_errors.h"
#include <stdbool.h>
#include <stdint.h>

#include <FreeRTOS.h>

#define THERMAL_MGR_PERIOD_MS 60000UL
#define THERMAL_MGR_PERIOD_TICKS pdMS_TO_TICKS(THERMAL_MGR_PERIOD_MS)

/**
 * @brief Sets the temperature part of temperatureData to temperature
 *
 * @param temperatureData Pointer to the variable containing temperature data
 * @param temperature Temperature value to set, most significant bit must be 0
 * @param isValid Boolean indicating whether the temperature data is valid
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if temperatureData is set successfully
 */
obc_error_code_t setTemperatureData(uint32_t *temperatureData, uint32_t temperature, bool isValid);

/**
 * @brief Sets temperature to the temperature part of temperatureData
 *
 * @param temperatureData Variable containing temperature data
 * @param temperature Pointer to the variable where the temperature will be stored
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if temperatureData is valid and temperature is set
 */
obc_error_code_t getTemperatureData(uint32_t temperatureData, uint32_t *temperature);

/**
 * @brief Sets isValid to the least significant bit of temperatureData
 *
 * @param temperatureData Variable containing temperature data
 * @param isValid Pointer to the variable where the validity will be stored
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if isValid is set
 */
obc_error_code_t isTemperatureValid(uint32_t temperatureData, bool *isValid);
