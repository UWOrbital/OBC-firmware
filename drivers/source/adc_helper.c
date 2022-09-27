#include "adc_helper.h"

#include <FreeRTOS.h>
#include <os_semphr.h>

static SemaphoreHandle_t adcMutex = NULL;

void InitADCMutex(void) {
    if(adcMutex == NULL) {
        adcMutex = xSemaphoreCreateMutex();
    }

}

static void adcStartConversion_selChn(adcBASE_t *adc, unsigned channel, unsigned fifo_size, unsigned group) {
    adc->GxINTCR[group] = fifo_size;

    adc->GxSEL[group] = 1 << channel;
}

static void adcGetSingleData(adcBASE_t *adc, unsigned group, adcData_t *data) {
    unsigned buf;
    adcData_t *ptr = data;

    buf = adc->GxBUF[group].BUF0;
    ptr->value = (unsigned short)(buf & 0xFFFU);
    ptr->id = (unsigned short)((buf >> 16U) & 0x1FU);

    adc->GxINTFLG[group] =9U;
}

static uint16_t adcDigitalVoltage(adcBASE_t *adc, unsigned group, unsigned channel) {
    adcData_t adc_data;
    adcData_t *adc_data_ptr = &adc_data;

    adcStartConversion_selChn(adc, channel, 6U, group);

    while(!adcIsConversionComplete(adc, group));

    adcGetSingleData(adc, group, adc_data_ptr);

    return adc_data.value;
}

float adcAnalogVoltage(adcBASE_t *adc, unsigned group, unsigned channel) {
    uint16_t digitalVoltage = adcDigitalVoltage(adc, group, channel);

    float analogvoltage = (float) ADC_RESOLUTION/ ((float) (digitalVoltage)) * 5.25;

    return analogvoltage;
}