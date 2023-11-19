#pragma once

#include "obc_errors.h"

#include <stdint.h>
#include <adc.h>

/**
 * @brief Initialize the ADC bus mutex
 */
void initADCMutex(void);

/**
 * @brief Get data from a single ADC channel
 * @param adc Pointer to adc module
 * @param channel Channel number
 * @param group Group number
 * @param data The buffer to receive into
 * @return OBC_ERR_CODE_INVALID_ARG if data or adc parameters are null, OBC_ERR_CODE_MUTEX_TIMEOUT
 * if unable to obtain ADC mutex, OBC_ERR_CODE_SUCCESS if conversion completed and data is obtained
 */
static obc_error_code_t adcGetSingleData(adcBASE_t *adc, uint8_t channel, uint8_t group, adcData_t *data);

/**
 * @brief Helper function for adcGetSingleData. Should never be called explicitly.
 * @param adc Pointer to adc module
 * @param group Group number
 * @param data The buffer to receive into
 * @return OBC_ERR_CODE_SUCCESS if memory read into data
 */
static obc_error_code_t adcReadSingleData(adcBASE_t *adc, uint8_t group, adcData_t *data);

/**
 * @brief Get data from an entire group (Calls functions from adc.c to get group values. Note that *data should have
 * adjacent buffers to hold all the data being requested. The group table and FIFO size should be hardcoded based on
 * used adc channels per group in adc.c for this to work properly.)
 * @param adc Pointer to adc module
 * @param channel Channel number
 * @param group Group number
 * @param data The buffer to receive into
 * @return OBC_ERR_CODE_INVALID_ARG if data or adc parameters are null, OBC_ERR_CODE_MUTEX_TIMEOUT
 * if unable to obtain ADC mutex, OBC_ERR_CODE_SUCCESS if conversiond completed and data is obtained
 */
static obc_error_code_t adcGetGroupData(adcBASE_t *adc, uint8_t channel, uint8_t group, adcData_t *data);
