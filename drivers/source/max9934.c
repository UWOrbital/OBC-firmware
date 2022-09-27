#include "max9934.h"
#include "adc_helper.h"

static uint16_t max9934ADCDigitalVoltage(void) {
    adcData_t adc_data;
    adcData_t *adc_data_ptr = &adc_data;

    adcStartConversion(MAX9934_ADC_REG, MAX9934_ADC_GROUP);

    while(!adcIsConversionComplete(MAX9934_ADC_REG, MAX9934_ADC_GROUP));

    uint32_t count = adcGetData(MAX9934_ADC_REG, MAX9934_ADC_GROUP, adc_data_ptr);

    uint16_t value;
    adcData_t *ptr = adc_data_ptr;

    for(uint8_t i = 0; i < count; i++) {
        if(ptr->id == MAX9934_PIN) {
            value = ptr->value;
            break;
        }
    }

    return value;

    

/*
    adcStartConversion_selChn(MAX9934_ADC_REG, MAX9934_PIN, 6, MAX9934_ADC_GROUP);

    while(!adcIsConversionComplete(MAX9934_ADC_REG, MAX9934_ADC_GROUP));

    adcGetSingleData(MAX9934_ADC_REG, MAX9934_ADC_GROUP, adc_data_ptr);
 
    return(adc_data_ptr->value); */
}

static float max9934ADCAnalogVoltage(void) {
    
    uint16_t digitalVoltage = max9934ADCDigitalVoltage();
    float analogvoltage = (float) ADC_RESOLUTION/ ((float) (digitalVoltage)) * 5.25; 

    return analogvoltage;
}

static void adcGetSingleData(adcBASE_t *adc, unsigned group, adcData_t *data) {
    unsigned buf;
    adcData_t *ptr = data;

    buf = adc->GxBUF[group].BUF0;
    ptr->value = (unsigned short)(buf & 0xFFFU);
    ptr->id = (unsigned short)((buf >> 16U) & 0x1FU);

    adc->GxINTFLG[group] =9U;
}

static void adcStartConversion_selChn(adcBASE_t *adc, unsigned channel, unsigned fifo_size, unsigned group) {
    adc->GxINTCR[group] = fifo_size;

    adc->GxSEL[group] = 1 << channel;
}

uint8_t getCurrentMAX9934(float *analogCurrent) {
    if (analogCurrent == NULL) {
        return 0;
    }

    /* MAX9934 Circuit Configuration */
    float rOut = 10.0; /* 10 kOhms */
    float rSense = 25.0; /* 25 kOhms */
    float gain = 25.0;  /* 25 uA/mV */

    float analogVoltage = max9934ADCAnalogVoltage();

    *analogCurrent = analogVoltage / (rOut * rSense * gain);

    return 1;
}

 
