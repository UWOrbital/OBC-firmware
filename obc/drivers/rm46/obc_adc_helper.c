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
}

static obc_error_code_t adcGetSingleData(adcBASE_t *adc, uint8_t channel, uint8_t group, adcData_t *data) {
  if (adc == NULL || data == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  ASSERT(adcConversionMutex != NULL);

  if (xSemaphoreTake(adcConversionMutex, portMAX_DELAY) != pdTRUE) {
    return OBC_ERR_CODE_MUTEX_TIMEOUT;
  }

  adc->GxSEL[group] = 1 << channel;

  volatile uint32_t *statusReg;
  if (group == 0) {
    statusReg = &(adc->EVSR);
  } else if (group == 1) {
    statusReg = &(adc->G1SR);
  } else {
    statusReg = &(adc->G2SR);
  }

  while (!(*statusReg & 0x1))
    ;

  adcStopConversion(adc, group);
  adcReadSingleData(adc, group, *data);

  xSemaphoreGive(adcConversionMutex);

  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t adcReadSingleData(adcBASE_t *adc, uint8_t group, adcData_t *data) {
  unsigned buf;
  adcData_t *ptr = data;

  buf = adc->GxBUF[group].BUF0;
  ptr->value = (unsigned short)(buf & 0xFFFU);
  ptr->id = (unsigned short)((buf >> 16U) & 0x1FU);

  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t adcGetGroupData(adcBASE_t *adc, uint8_t channel, uint8_t group, adcData_t *data) {
  if (adc == NULL || data == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  if (xStaticSemaphoreTake(adcConversionMutexBuffer, portMAX_DELAY) != pdTRUE) {
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
