#ifndef DRIVERS_INCLUDE_MAX9934_H_
#define DRIVERS_INCLUDE_MAX9934_H_

#include <adc.h>

/* MAX9934 pin assignment */
#define MAX9934_ADC_GROUP adcGROUP1
#define MAX9934_PIN 0U
#define MAX9934_ADC_REG adcREG1
#define MAX9934_GAIN 25.0 //  25 uA/mV


typedef struct {
    uint32 numConversions;
    uint16_t digitalValue;
    float analogValue;
} max9934_config_t;


/**
 * @brief Initialize the MAX9934
 * 
 * @param config Configuration struct for MAX9934
 * @return 1 if successful, 0 otherwise
 */

uint8_t max9934init(max9934_config_t *config);

/**
 * @brief Read the current from the max9334
 * 
 * @param Digital voltage value output from the ADC
 * @param Number of converted values copied into data buffer
 * 
 * @return 1 if successful, 0 otherwise
 */

uint8_t getCurrentSensorDigitalValue(uint16_t *digitalValue, uint32 *numConversions);

/**
 * @brief Convert value from ADC to Analog Voltage Measured
 * 
 * @param Converted voltage analog value output from the digital value output from the ADC
 * @param Digital voltage value output from the ADC
 * @param Number of converted values copied into data buffer

 * @return 1 if successful, 0 otherwise
 */

uint8_t getCurrentSensorAnalogValue(float *analogValue, uint16_t *digitalValue, uint32 *numConversions);

/**
 * @brief Convert ADC analog voltage measured to current value in uA using the gain from the max9934
 * 
 * @param Analog voltage value output from the ADC
 * @param Digital voltage value output from the ADC
 * @param Number of converted values copied into data buffer
 * @param Analog current value output from the max9934

 * @return 1 if successful, 0 otherwise
 */

uint8_t getCurrentValue(float *analogValue, uint16_t *digitalValue, uint32 *numConversions, float *analogCurrentValue);

#endif /* DRIVERS_INCLUDE_MAX9934_H_ */