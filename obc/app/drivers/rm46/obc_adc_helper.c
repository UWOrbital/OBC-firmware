#include "obc_logging.h"
#include "obc_errors.h"
#include "adc.h"
#include "reg_adc.h"
#include "obc_adc_helper.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_semphr.h>
#include <os_task.h>

static SemaphoreHandle_t adcConversionMutex = NULL;
static StaticSemaphore_t adcConversionMutexBuffer;

const uint32_t adcGroupLengths[2U][3U] = {{1U, 2U, 1U}, {1U, 1U, 1U}};

obc_error_code_t initADCMutex(void) {
  if (adcConversionMutex == NULL) {
    adcConversionMutex = xSemaphoreCreateMutexStatic(&adcConversionMutexBuffer);
  }
  ASSERT(adcConversionMutex != NULL);
  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t adcGetGroupReadings(ADC_module_t adc, ADC_group_t group, adcData_t *data,
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
  while (!adcIsConversionComplete(adcReg, group) && totalAttempts < 6) {
    if (totalAttempts >= 5) {
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

obc_error_code_t adcGetSingleData(ADC_module_t adc, ADC_channel_t channel, ADC_group_t group, float *reading,
                                  TickType_t blockTime) {
  if (reading == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  if (adcConversionMutex == NULL) {
    return OBC_ERR_CODE_INVALID_STATE;
  }

  adcData_t adcData[MAXGROUPSIZE];

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

obc_error_code_t adcGetGroupData(ADC_module_t adc, ADC_group_t group, float *readings, TickType_t blockTime) {
  if (readings == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  if (adcConversionMutex == NULL) {
    return OBC_ERR_CODE_INVALID_STATE;
  }

  adcData_t adcData[MAXGROUPSIZE];

  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(adcGetGroupReadings(adc, group, adcData, blockTime));

  uint32_t groupSize = adcGroupLengths[(adc == ADC1) ? 0U : 1U][group];

  for (uint32_t i = 0; i < groupSize; i++) {
    readings[i] =
        (float)(adcData[i].value) * (REF_VOLTAGE_HIGH - REF_VOLTAGE_LOW) / ((float)(1 << RESOLUTION) - REF_VOLTAGE_LOW);
  }

  return OBC_ERR_CODE_SUCCESS;
}
