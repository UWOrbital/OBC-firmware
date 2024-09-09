#pragma once

#include "obc_logging.h"
#include "obc_errors.h"
#include "adc.h"
#include "reg_adc.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_semphr.h>
#include <os_task.h>

#include <stdint.h>

typedef enum { ADC1 = 0U, ADC2 } ADC_module_t;

typedef enum { EVENT = 0U, GROUP1, GROUP2 } ADC_group_t;

typedef enum {
  ADC_CHANNEL_0 = 0U,
  ADC_CHANNEL_1,
  ADC_CHANNEL_2,
  ADC_CHANNEL_3,
  ADC_CHANNEL_4,
  ADC_CHANNEL_5,
  ADC_CHANNEL_6,
  ADC_CHANNEL_7,
  ADC_CHANNEL_8,
  ADC_CHANNEL_9,
  ADC_CHANNEL_10,
  ADC_CHANNEL_11,
  ADC_CHANNEL_12,
  ADC_CHANNEL_13,
  ADC_CHANNEL_14,
  ADC_CHANNEL_15,
  ADC_CHANNEL_16,
  ADC_CHANNEL_17,
  ADC_CHANNEL_18,
  ADC_CHANNEL_19,
  ADC_CHANNEL_20,
  ADC_CHANNEL_21,
  ADC_CHANNEL_22,
  ADC_CHANNEL_23
} ADC_channel_t;

/**
 * @brief Initialize the ADC bus mutex
 */
void initADCMutex(void);

/**
 * @note Get data from a single ADC channel. ADC conversion mode channel ID should be ENABLED for all groups this is
 * called for.
 * @param adc Pointer to adc module
 * @param channel Channel number
 * @param group Group number (0-2)
 * @param reading The float to receive the analog reading
 * @param blockTime The HAL ticks to wait for mutex
 * @return OBC_ERR_CODE_INVALID_ARG if reading or adc parameters are null,
 * OBC_ERR_CODE_INVALID_STATE if ADC mutex not init,
 * OBC_ERR_CODE_MUTEX_TIMEOUT if unable to obtain ADC mutex,
 * OBC_ERR_CODE_SUCCESS if conversion completed and analog data is obtained,
 * OBC_ERR_CODE_ADC_INVALID_CHANNEL if conversion completed but channel number is not found
 */
obc_error_code_t adcGetSingleData(ADC_module_t adc, ADC_channel_t channel, ADC_group_t group, float *reading,
                                  TickType_t blockTime);

/**
 * @note Get data from an entire group, must pass in an array of floats long enough to hold values for the whole group
 * @param adc Pointer to adc module
 * @param group Group number (0-2)
 * @param readings Array of floats equal to number of channels in the group
 * @param blockTime The HAL ticks to wait for mutex
 * @return OBC_ERR_CODE_INVALID_ARG if data or adc parameters are null,
 * OBC_ERR_CODE_INVALID_STATE if ADC mutex not init,
 * OBC_ERR_CODE_MUTEX_TIMEOUT if unable to obtain ADC mutex,
 * OBC_ERR_CODE_SUCCESS if conversions completed and analog data is obtained
 */
obc_error_code_t adcGetGroupData(ADC_module_t adc, ADC_group_t group, float *readings, TickType_t blockTime);