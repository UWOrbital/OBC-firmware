#ifndef DRIVERS_INCLUDE_MAX9934_H_
#define DRIVERS_INCLUDE_MAX9934_H_

#include <adc.h>

/* MAX9934 ADC pin configuration*/
#define MAX9934_ADC_GROUP adcGROUP1
#define MAX9934_PIN 0U
#define MAX9934_ADC_REG adcREG1

/**
 * @brief Convert ADC analog voltage measured to current value in uA
 *
 * @param Analog current value output from the max9934 in uA

 * @return 1 if successful, 0 otherwise
 */

uint8_t getCurrentMAX9934(float *analogCurrent);

/**
 * @brief Read ADC digital voltage value

 * @return Digital voltage read by ADC from MAX9934
 */

static uint16_t max9934ADCDigitalVoltage(void);

/**
 * @brief Converts ADC digital voltage to equivalant analog voltage from MAX9934

 * @return Analog voltage value from MAX9934
 */

static float max9934ADCAnalogVoltage(void);

/**
 * @brief ADC helper functions for single channel ADC conversion
 */

static void adcGetSingleData(adcBASE_t *adc, unsigned group, adcData_t *data);

static void adcStartConversion_selChn(adcBASE_t *adc, unsigned channel, unsigned fifo_size, unsigned group);

#endif /* DRIVERS_INCLUDE_MAX9934_H_ */