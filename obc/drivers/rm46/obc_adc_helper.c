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

obc_error_code_t adcGetSingleData(adcBASE_t *adc, uint8_t channel, uint8_t group, float *reading,
                                  TickType_t blockTime) {
  if (adc == NULL || reading == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  if (adcConversionMutex == NULL) {
    return OBC_ERR_CODE_INVALID_STATE;
  }

  adcData_t adcData[MAXGROUPSIZE];

  if (adcGetGroupReadings(adc, group, adcData, blockTime) != OBC_ERR_CODE_SUCCESS) {
    return OBC_ERR_CODE_MUTEX_TIMEOUT;
  }

  adcData_t *ptr = adcData;
  uint32_t groupSize = adcGroupSize[(adc == adcREG1) ? 0U : 1U, group];
  for (int i = 0; i < groupSize; i++) {
    if (ptr->id == channel) {
      *reading = (float)(1 << RESOLUTION) / ((float)(ptr->value)) * 5.00;
      return OBC_ERR_CODE_SUCCESS;
    }
    ptr++;
  }

  return OBC_ERR_CODE_ADC_INVALID_CHANNEL;
}

obc_error_code_t adcGetGroupData(adcBASE_t *adc, uint8_t group, float *readings, TickType_t blockTime) {
  if (adc == NULL || readings == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  if (adcConversionMutex == NULL) {
    return OBC_ERR_CODE_INVALID_STATE;
  }

  adcData_t adcData[MAXGROUPSIZE];

  if (adcGetGroupReadings(adc, group, adcData, blockTime) != OBC_ERR_CODE_SUCCESS) {
    return OBC_ERR_CODE_MUTEX_TIMEOUT;
  }

  adcData_t *ptr = adcData;
  float *readingPtr = readings;
  uint32_t groupSize = adcGroupSize[(adc == adcREG1) ? 0U : 1U, group];

  for (int i = 0; i < groupSize; i++) {
    *readingPtr = (float)(1 << RESOLUTION) / ((float)(ptr->value)) * 5.00;
    ptr++;
    readingPtr++;
  }

  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t adcGetGroupReadings(adcBASE_t *adc, uint8_t group, adcData_t *data, TickType_t blockTime) {
  if (adc == NULL || data == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  if (xStaticSemaphoreTake(adcConversionMutexBuffer, blockTime) != pdTRUE) {
    return OBC_ERR_CODE_MUTEX_TIMEOUT;
  }

  adcStartConversion(adc, group);

  while (!adcIsConversionComplete(adc, group))
    ;

  adcStopConversion(adc, group);

  adcGetData(adc, group, data);

  adcResetFiFo(*adc, group);

  xSemaphoreGive(adcConversionMutex);

  return OBC_ERR_CODE_SUCCESS;
}
