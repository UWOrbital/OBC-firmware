#include "obc_logging.h"
#include "obc_errors.h"
#include "obc_adc_helper.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_semphr.h>
#include <os_task.h>

#include <adc.h>

static SemaphoreHandle_t adcConversionMutex = NULL;
static StaticSemaphore_t adcConversionMutexBuffer;

void initADCMutex(void) {
  if (adcConversionMutex == NULL) {
    adcConversionMutex = xSemaphoreCreateMutexStatic(&adcConversionMutexBuffer);
  }
  ASSERT(adcConversionMutex != NULL);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t adcGetSingleData(ADC_module_t adc, uint8_t channel, ADC_group_t group, float *reading,
                                  TickType_t blockTime) {
  if (adc == NULL || reading == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  if (adcConversionMutex == NULL) {
    return OBC_ERR_CODE_INVALID_STATE;
  }
  adcBASE_t *adcReg = (adc == ADC1) ? (adcBASE_t *)0xFFF7C000U : (adcBASE_t *)0xFFF7C200U;

  adcData_t adcData[MAXGROUPSIZE];

  if (adcGetGroupReadings(adcReg, group, adcData, blockTime) != OBC_ERR_CODE_SUCCESS) {
    return OBC_ERR_CODE_MUTEX_TIMEOUT;
  }

  adcData_t *ptr = &adcData[0];
  uint32_t groupSize = adcGroupSize[(adc == adcREG1) ? 0U : 1U, group];
  for (int i = 0; i < groupSize; i++) {
    if (ptr->id == channel) {
      *reading = (float)(1 << RESOLUTION) / ((float)(*ptr->value)) * 5.00;
      return OBC_ERR_CODE_SUCCESS;
    }
    ptr++;
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
  RETURN_IF_ERROR_CODE((adcGetGroupReadings(adc, group, adcData, blockTime)))

  adcData_t *ptr = &adcData[0];
  float *readingPtr = &readings[0];
  uint32_t groupSize = adcGroupSize[(adc == adcREG1) ? 0U : 1U, group];

  for (int i = 0; i < groupSize; i++) {
    *readingPtr = (float)(1 << RESOLUTION) / ((float)(*ptr->value)) * 5.00;
    ptr++;
    readingPtr++;
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
  // ADC HAL functions expect group value to be a uint32
  uint32_t groupNum = (group == EVENT) ? 0U : (group == GROUP1) ? 1U : 2U;

  adcStartConversion(adcReg, groupNum);

  while (!adcIsConversionComplete(adcReg, groupNum))
    ;

  adcStopConversion(adcReg, groupNum);

  adcGetData(adcReg, groupNum, data);

  xSemaphoreGive(adcConversionMutex);

  return OBC_ERR_CODE_SUCCESS;
}
