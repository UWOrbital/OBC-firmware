#ifndef ADC_HELPER_FUNCTIONS_H
#define ADC_HELPER_FUNCTIONS_H

#include <stdint.h>
#include <adc.h>

#define ADC_RESOLUTION 4095U
#define FIFOSIZE 6U

/**
 * @brief Function to get ADC analog voltage
 * 
 * @param adc Pointer to ADC Module
 *        - adcREG1: ADC1 module pointer
 *        - adcREG2: ADC2 module pointer
 * @param group ADC group to be converted
 * @param channel ADC channel to be converted
 * @param adcValue Pointer to converted analog voltage
 * 
 * @return 1 if successful, 0 if not 
 */ 

uint8_t adcAnalogVoltage(adcBASE_t *adc, uint8_t group, uint8_t channel, float *adcValue);

#endif