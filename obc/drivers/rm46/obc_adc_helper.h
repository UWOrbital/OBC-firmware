#pragma once

#include "obc_logging.h"
#include "obc_errors.h"
#include "obc_adc_helper.h"
#include "reg_adc.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_semphr.h>
#include <os_task.h>

#include <adc.h>
#include <stdint.h>

// Should be configured to max FIFO size for all groups, so that an adcData_t array of a proper size can be made
#define MAXGROUPSIZE 16U
// Assumes all groups have same resolution for simplicity. Could make this more flexible/an enum.
#define RESOLUTION 12U

#define ADC_CHANNEL_0 0U
#define ADC_CHANNEL_1 1U
#define ADC_CHANNEL_2 2U
#define ADC_CHANNEL_3 3U
#define ADC_CHANNEL_4 4U
#define ADC_CHANNEL_5 5U
#define ADC_CHANNEL_6 6U
#define ADC_CHANNEL_7 7U
#define ADC_CHANNEL_8 8U
#define ADC_CHANNEL_9 9U
#define ADC_CHANNEL_10 10U
#define ADC_CHANNEL_11 11U
#define ADC_CHANNEL_12 12U
#define ADC_CHANNEL_13 13U
#define ADC_CHANNEL_14 14U
#define ADC_CHANNEL_15 15U
#define ADC_CHANNEL_16 16U
#define ADC_CHANNEL_17 17U
#define ADC_CHANNEL_18 18U
#define ADC_CHANNEL_19 19U
#define ADC_CHANNEL_20 20U
#define ADC_CHANNEL_21 21U
#define ADC_CHANNEL_22 22U
#define ADC_CHANNEL_23 23U

typedef enum { ADC1, ADC2 } ADC_module_t;

typedef enum { EVENT, GROUP1, GROUP2 } ADC_group_t;

// Hardcoded table. # of channels in each Group 0-2 for ADC modules 1-2. All set to 1 for testing purposes.
// **IMPORTANT** Should be updated to reflect the amount of channels assigned to each group of both ADC modules
const uint32_t adcGroupSize[2U][3U] = {{1U, 1U, 1U}, {1U, 1U, 1U}};

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
obc_error_code_t adcGetSingleData(adcBASE_t *adc, uint8_t channel, uint8_t group, float *reading, TickType_t blockTime);

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
obc_error_code_t adcGetGroupData(adcBASE_t *adc, uint8_t group, float *readings, TickType_t blockTime);
