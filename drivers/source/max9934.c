#include "max9934.h"
#include "adc_helper.h"

uint8_t getCurrentMAX9934(float *analogCurrent) {
    if (analogCurrent == NULL) {
        return 0;
    }

    /* MAX9934 Circuit Configuration */
    float rOut = 10.0; /* 10 kOhms */
    float rSense = 25.0; /* 25 kOhms */
    float gain = 25.0;  /* 25 uA/mV */

    float analogVoltage = adcAnalogVoltage(MAX9934_ADC_REG, MAX9934_ADC_GROUP, MAX9934_PIN);

    *analogCurrent = analogVoltage / (rOut * rSense * gain);

    return 1;
}

 
