#include "obc_logging.h"
#include "stdint.h"
#include "adc.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_semphr.h>
#include <os_task.h>

static SemaphoreHandle_t adcConversionMutex = NULL;
static StaticSemaphore_t adcConversionMutexBuffer;

static SemaphoreHandle_t adcConversionComplete = NULL;
static StaticSemaphore_t adcConversionCompleteBuffer;

void initADCMutex(void) {
  if (adcConversionMutex == NULL) {
    adcConversionMutex = xSemaphoreCreateMutexStatic(&adcConversionMutexBuffer);
  }
  ASSERT(adcConversionMutex != NULL);
  if (adcConversionComplete == NULL) {
    adcConversionComplete = xSemaphoreCreateBinaryStatic(&adcConversionCompleteBuffer);
  }
  ASSERT(adcConversionComplete != NULL);
}

static obc_error_code_t adcGetSingleChData(adcBASE_t *adc, uint8_t channel, uint8_t group, adcData_t *data) {
  if (adc == NULL || data == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  adc->GxSEL[group] = 1 << channel;
  // GxSR sets bit 0 to 1 when group conversions are done: recommended for single conversion mode
  volatile uint32_t *statusReg;
  if (group == 0) {
    statusReg = &(adc->EVSR);
  } else if (group == 1) {
    statusReg = &(adc->G1SR);
  } else {
    statusReg = &(adc->G2SR);
  }
  // While loop runs until end flag is set to 1
  while (!(*statusReg & 0x1))
    ;

  adcStopConversion(adc, group);
  adcReadSingleData(adc, group, *data);

  return OBC_ERR_CODE_SUCCESS;
}

// Helper function for adcGetSingleChData
static obc_error_code_t adcReadSingleData(adcBASE_t *adc, uint8_t group, adcData_t *data) {
  unsigned buf;
  adcData_t *ptr = data;

  buf = adc->GxBUF[group].BUF0;
  ptr->value = (unsigned short)(buf & 0xFFFU);
  ptr->id = (unsigned short)((buf >> 16U) & 0x1FU);

  return OBC_ERR_CODE_SUCCESS;
}

// A helper function: calls functions from adc.c to get group values. Note that *data should have adjacent buffers to
// hold all the data being requested. The group table and FIFO size should be hardcoded based on used adc channels per
// group in adc.c for this to work properly.
static obc_error_code_t adcGetGroupData(adcBASE_t *adc, uint8_t channel, uint8_t group, adcData_t *data) {
  if (adc == NULL || data == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  adcStartConversion(adc, group);

  while (!adcIsConversionComplete(adc, group))
    ;
  adcStopConversion(adc, group);

  adcGetData(adc, group, data);
  adcResetFiFo(*adc, group);

  return OBC_ERR_CODE_SUCCESS;
}
