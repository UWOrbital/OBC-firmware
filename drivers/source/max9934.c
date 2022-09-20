#include "max9934.h"
#include "obc_i2c_io.h"


uint8_t max9934init(max9934_config_t *config) {
    if (config == NULL) {
        return 0;
    }

    return 1;
}

uint8_t max9934ADCDigitalVoltage(uint16_t *digitalVoltage, uint32 *numConversions) {
    if ((digitalVoltage == NULL) || (numConversions == NULL)) {
        return 0;
    }
    
    adcData_t adc_data;

    adcStartConversion(MAX9934_ADC_REG, MAX9934_ADC_GROUP);

    while(!adcIsConversionComplete(MAX9934_ADC_REG, MAX9934_ADC_GROUP));

    *numConversions = adcGetData(MAX9934_ADC_REG, MAX9934_ADC_GROUP, &adc_data);

    *digitalVoltage = adc_data.value;

    return 1;
}

uint8_t max9934ADCAnalogVoltage(float *analogVoltage, uint16_t *digitalVoltage, uint32 *numConversions) {
    if ((analogVoltage == NULL) || (digitalVoltage == NULL) || (numConversions == NULL)) {
        return 0;
    }
    
    if (max9934ADCDigitalVoltage(digitalVoltage, numConversions) == 0) {
        return 0;
    }

    *analogVoltage = (float) ADC_12_BIT/(*digitalVoltage) * 5.25; 

    return 1;
}

uint8_t max9934MeasuredCurrent(float *analogVoltage, uint16_t *digitalValue, uint32 *numConversions, float *analogCurrent) {
    if ((analogVoltage == NULL) || (digitalValue == NULL) || (numConversions == NULL) || (analogCurrent == NULL)) {
        return 0;
    }

    if (max9934ADCAnalogVoltage(analogVoltage, digitalValue, numConversions) == 0) {
        return 0;
    }

    /* MAX9934 Circuit Configuration */
    float rOut = 10.0; 
    float rSense = 25.0;
    float gain = 25.0; 

    *analogCurrent = *analogVoltage / (rOut * rSense * gain);


    return 1;
}


 
