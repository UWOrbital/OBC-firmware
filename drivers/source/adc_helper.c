#include "adc_helper.h"

#include <FreeRTOS.h>
#include <os_semphr.h>

static void adcStartConversion_selChn(adcBASE_t *adc, uint8_t channel, uint8_t fifoSize, uint8_t group) {
    adc->GxINTCR[group] = fifoSize;

    adc->GxSEL[group] = 1 << channel;
}

static void adcGetSingleData(adcBASE_t *adc, uint8_t group, adcData_t *data) {
    unsigned buf;
    adcData_t *ptr = data;

    buf = adc->GxBUF[group].BUF0;
    ptr->value = (unsigned short)(buf & 0xFFFU);
    ptr->id = (unsigned short)((buf >> 16U) & 0x1FU);

    adc->GxINTFLG[group] = 9U;
}

static uint16_t adcDigitalVoltage(adcBASE_t *adc, uint8_t group, uint8_t channel) {
    adcData_t adcData;
    
    adcStartConversion_selChn(adc, channel, FIFOSIZE, group);

    while(!adcIsConversionComplete(adc, group));

    adcGetSingleData(adc, group, &adcData);

    return adcData.value;
}

float adcAnalogVoltage(adcBASE_t *adc, uint8_t group, uint8_t channel) {
    uint16_t digitalVoltage = adcDigitalVoltage(adc, group, channel);

    float analogVoltage = (float) ADC_RESOLUTION/ ((float) (digitalVoltage)) * 5.25;

    return analogVoltage;
}