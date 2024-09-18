#include "obc_logging.h"
#include "obc_errors.h"
#include "adc.h"
#include "reg_adc.h"
#include "obc_adc.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_semphr.h>
#include <os_task.h>

static SemaphoreHandle_t adcConversionMutex = NULL;
static StaticSemaphore_t adcConversionMutexBuffer;

// Should be configured to max FIFO size for all groups, so that an adcData_t array of a proper size can be made
#define MAX_GROUP_SIZE 16U

// Assumes all groups have same resolution for simplicity. Could make this more flexible/an enum.
#define RESOLUTION 12U

// VCCAD is 3.3V
#define REF_VOLTAGE_HIGH 3.3f
#define REF_VOLTAGE_LOW 0.0f

// Arbitrary
#define ADC_MAX_WAIT_CYCLES 5

typedef enum {
  ADC1_GROUP0_NUM_CHANNELS = 16U,
  ADC1_GROUP1_NUM_CHANNELS = 16U,
  ADC1_GROUP2_NUM_CHANNELS = 16U,
  ADC2_GROUP0_NUM_CHANNELS = 16U,
  ADC2_GROUP1_NUM_CHANNELS = 16U,
  ADC2_GROUP2_NUM_CHANNELS = 16U
} adc_group_channels_t;

const uint32_t adcGroupLengths[2U][3U] = {
    {ADC1_GROUP0_NUM_CHANNELS, ADC1_GROUP1_NUM_CHANNELS, ADC1_GROUP2_NUM_CHANNELS},
    {ADC2_GROUP0_NUM_CHANNELS, ADC2_GROUP1_NUM_CHANNELS, ADC2_GROUP2_NUM_CHANNELS}};

void initADC(void) {
  adcInit();
  if (adcConversionMutex == NULL) {
    adcConversionMutex = xSemaphoreCreateMutexStatic(&adcConversionMutexBuffer);
  }
  ASSERT(adcConversionMutex != NULL);
}

static obc_error_code_t adcGetGroupReadings(adc_module_t adc, adc_group_t group, adcData_t *data,
                                            TickType_t blockTime) {
  if (data == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  if (xSemaphoreTake(adcConversionMutex, blockTime) != pdTRUE) {
    return OBC_ERR_CODE_MUTEX_TIMEOUT;
  }

  adcBASE_t *adcReg = (adc == ADC1) ? adcREG1 : adcREG2;

  adcStartConversion(adcReg, group);

  uint8_t totalAttempts = 0;
  while (!adcIsConversionComplete(adcReg, group)) {
    if (totalAttempts >= ADC_MAX_WAIT_CYCLES) {
      adcStopConversion(adcReg, group);
      xSemaphoreGive(adcConversionMutex);
      return OBC_ERR_CODE_ADC_FAILURE;
    }
    totalAttempts++;
  }

  adcStopConversion(adcReg, group);

  adcGetData(adcReg, group, data);

  xSemaphoreGive(adcConversionMutex);

  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t adcGetSingleData(adc_module_t adc, adc_channel_t channel, adc_group_t group, float *reading,
                                  TickType_t blockTime) {
  if (reading == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  if (adcConversionMutex == NULL) {
    return OBC_ERR_CODE_INVALID_STATE;
  }

  adcData_t adcData[MAX_GROUP_SIZE];

  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(adcGetGroupReadings(adc, group, adcData, blockTime));

  uint32_t groupSize = adcGroupLengths[(adc == ADC1) ? 0U : 1U][group];

  for (uint32_t i = 0; i < groupSize; i++) {
    if (adcData[i].id == channel) {
      *reading = (float)(adcData[i].value) * (REF_VOLTAGE_HIGH - REF_VOLTAGE_LOW) /
                 ((float)(1 << RESOLUTION) - REF_VOLTAGE_LOW);
      return OBC_ERR_CODE_SUCCESS;
    }
  }

  return OBC_ERR_CODE_ADC_INVALID_CHANNEL;
}

obc_error_code_t adcGetGroupData(adc_module_t adc, adc_group_t group, float *readings, TickType_t blockTime) {
  if (readings == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  if (adcConversionMutex == NULL) {
    return OBC_ERR_CODE_INVALID_STATE;
  }

  adcData_t adcData[MAX_GROUP_SIZE];

  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(adcGetGroupReadings(adc, group, adcData, blockTime));

  uint32_t groupSize = adcGroupLengths[(adc == ADC1) ? 0U : 1U][group];

  for (uint32_t i = 0; i < groupSize; i++) {
    readings[i] =
        (float)(adcData[i].value) * (REF_VOLTAGE_HIGH - REF_VOLTAGE_LOW) / ((float)(1 << RESOLUTION)) + REF_VOLTAGE_LOW;
  }

  return OBC_ERR_CODE_SUCCESS;
}
