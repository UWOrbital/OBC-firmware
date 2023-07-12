#ifndef DRIVERS_INCLUDE_OBC_ADC_IO_H_
#define DRIVERS_INCLUDE_OBC_ADC_IO_H_

#include "obc_errors.h"

#include <stdint.h>

#include <adc.h>
#include <os_projdefs.h>

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
 * @return OBC_ERR_CODE_SUCCESS if conversion was successful, else an error code
 */ 

obc_error_code_t adcAnalogVoltage(adcBASE_t *adc, uint8_t group, uint8_t channel, float *adcValue);

#endif /* DRIVERS_INCLUDE_OBC_ADC_IO_H_ */