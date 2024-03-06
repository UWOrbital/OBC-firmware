#include "obc_adc_helper.h"

static SemaphoreHandle_t adcConversionMutex = NULL;
static StaticSemaphore_t adcConversionMutexBuffer;

void initADCMutex(void) {
  if (adcConversionMutex == NULL) {
    adcConversionMutex = xSemaphoreCreateMutexStatic(&adcConversionMutexBuffer);
  }
  ASSERT(adcConversionMutex != NULL);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t adcGetSingleData(ADC_module_t adc, uint32_t channel, ADC_group_t group, float *reading,
                                  TickType_t blockTime) {
  if (adc == NULL || reading == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  if (adcConversionMutex == NULL) {
    return OBC_ERR_CODE_INVALID_STATE;
  }

  adcData_t adcData[MAXGROUPSIZE];

  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(adcGetGroupReadings(adc, group, adcData, blockTime));

  uint32_t groupSize = adcGroupSize[(adc == ADC1) ? 0U : 1U, group];

  for (int i = 0; i < groupSize; i++) {
    if (adcData[i].id == channel) {
      *reading =
          (float)(adcData[i].value) * (REF_VOLTAGE_HIGH - REF_VOLTAGE_LOW) / (float)(1 << RESOLUTION) - REF_VOLTAGE_LOW;
      return OBC_ERR_CODE_SUCCESS;
    }
  }

  return OBC_ERR_CODE_ADC_INVALID_CHANNEL;
}

obc_error_code_t adcGetGroupData(ADC_module_t adc, ADC_group_t group, float *readings, TickType_t blockTime) {
  if (adc == NULL || readings == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  if (adcConversionMutex == NULL) {
    return OBC_ERR_CODE_INVALID_STATE;
  }

  adcData_t adcData[MAXGROUPSIZE];

  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(adcGetGroupReadings(adc, group, adcData, blockTime));

  uint32_t groupSize = adcGroupSize[(adc == ADC1) ? 0U : 1U, group];

  for (int i = 0; i < groupSize; i++) {
    readings[i] =
        (float)(adcData[i].value) * (REF_VOLTAGE_HIGH - REF_VOLTAGE_LOW) / (float)(1 << RESOLUTION) - REF_VOLTAGE_LOW;
  }

  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t adcGetGroupReadings(ADC_module_t adc, ADC_group_t group, adcData_t *data,
                                            TickType_t blockTime) {
  if (adc == NULL || data == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  if (xStaticSemaphoreTake(adcConversionMutexBuffer, blockTime) != pdTRUE) {
    return OBC_ERR_CODE_MUTEX_TIMEOUT;
  }

  adcBASE_t *adcReg = (adc == ADC1) ? adcREG1 : adcREG2;

  adcStartConversion(adcReg, group);

  while (!adcIsConversionComplete(adcReg, group))
    ;

  adcStopConversion(adcReg, group);

  adcGetData(adcReg, group, data);

  xSemaphoreGive(adcConversionMutex);

  return OBC_ERR_CODE_SUCCESS;
}
