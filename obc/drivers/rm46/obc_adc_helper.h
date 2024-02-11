#pragma once

#include "obc_logging.h"
#include "obc_errors.h"
#include "obc_adc_helper.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_semphr.h>
#include <os_task.h>

#include <adc.h>
#include <stdint.h>

// Should be configured to max FIFO size for all groups, so that an adcData_t array of a proper size can be made
#define MAXGROUPSIZE 16U
// Assumes all groups have same resolution for simplicity. Could make this more flexible/not a macro.
#define RESOLUTION 12U

// Hardcoded table. # of channels in Groups 0-2 for ADC modules 1-2. Update as necessary.
const uint32_t adcGroupSize[2U][3U] = {{16U, 16U, 16U}, {16U, 16U, 16U}};

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
