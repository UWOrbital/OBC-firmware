#ifndef DRIVERS_INCLUDE_MAX9934_H_
#define DRIVERS_INCLUDE_MAX9934_H_

#include <stdio.h>
#include <adc.h>

/* MAX9934 ADC pin configuration*/
#define MAX9934_ADC_GROUP adcGROUP1
#define MAX9934_PIN 0U
#define MAX9934_ADC_REG adcREG1

/* MAX9934 Circuit Configuration */
#define ROUT 10.0f /* 10 kOhms */
#define RSENSE 25.0f /* 25 kohms */
#define GAIN 25.0f /* 25 uA/mV */

/**
 * @brief Convert ADC analog voltage measured to current value in uA
 *
 * @param Analog current value output from the max9934 in uA

 * @return 1 if successful, 0 otherwise
 */

uint8_t getCurrentMAX9934(float *analogCurrent);


#endif /* DRIVERS_INCLUDE_MAX9934_H_ */