#include "max9934.h"
#include "adc_helper.h"

uint8_t getCurrentMAX9934(float *analogCurrent) {
    if (analogCurrent == NULL) {
        return 0;
    }

    float analogVoltage;
    
    adcAnalogVoltage(MAX9934_ADC_REG, MAX9934_ADC_GROUP, MAX9934_PIN, &analogVoltage);

    *analogCurrent = analogVoltage / (ROUT * RSENSE * GAIN);

    return 1;
}

 
