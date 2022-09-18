#include "max9934.h"
#include "obc_i2c_io.h"

uint8_t max9934init(max9934_config_t *config) {
    if (config == NULL) {
        return 0;
    }

    return 1;
}

uint8_t getCurrentSensorDigitalValue(uint16_t *digitalValue, uint32 *numConversions) {
    if ((digitalValue == NULL) || (numConversions == NULL)) {
        return 0;
    }
    
    adcData_t adc_data;

    adcStartConversion(MAX9934_ADC_REG, MAX9934_ADC_GROUP);

    while(!adcIsConversionComplete(MAX9934_ADC_REG, MAX9934_ADC_GROUP));

    *numConversions = adcGetData(MAX9934_ADC_REG, MAX9934_ADC_GROUP, &adc_data);

    *digitalValue = adc_data.value;

    return 1;
}

uint8_t getCurrentSensorAnalogValue(float *analogValue, uint16_t *digitalValue, uint32 *numConversions) {
    if ((analogValue == NULL) || (digitalValue == NULL) || (numConversions == NULL)) {
        return 0;
    }
    
    uint16_t adcValue = getCurrentSensorDigitalValue(digitalValue, numConversions);

    *analogValue = (float) ADC_12_BIT/adcValue * 5.25; 

    return 1;
}

uint8_t getCurrentValue(float *analogValue, uint16_t *digitalValue, uint32 *numConversions, float *analogCurrentValue) {
    if ((analogValue == NULL) || (digitalValue == NULL) || (numConversions == NULL) || (analogCurrentValue == NULL)) {
        return 0;
    }

    uint8_t adcValue = getCurrentSensorAnalogValue(analogValue, digitalValue, numConversions);

    *analogCurrentValue = (float) MAX9934_GAIN * (*analogValue);

    return 1;
}


 
