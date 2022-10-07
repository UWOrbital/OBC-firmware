#ifndef ADC_HELPER_FUNCTIONS_H
#define ADC_HELPER_FUNCTIONS_H

#include <stdint.h>
#include <adc.h>

#define ADC_RESOLUTION 4095U
#define FIFOSIZE 6U

/**
 * @brief Function to start conversion of a single adc channel
 * 
 * @param adc Pointer to ADC Module
 *        - adcREG1: ADC1 module pointer
 *        - adcREG2: ADC2 module pointer
 * @param channel ADC channel to be converted
 * @param fifo_size Size of FIFO queue to store converted values
 * @param group ADC group to be converted
 * 
 * @return 1 if successful, 0 if not 
 */ 

static uint8_t adcStartConversion_selChn(adcBASE_t *adc, uint8_t channel, uint8_t fifoSize, uint8_t group);

/**
 * @brief Function to get ADC converted data
 * 
 * @param adc Pointer to ADC Module
 *        - adcREG1: ADC1 module pointer
 *        - adcREG2: ADC2 module pointer
 * @param group ADC group to get converted data from
 * @param data Pointer to store ADC converted data
 * 
 * @return 1 if successful, 0 if not 
 */ 

static uint8_t adcGetSingleData(adcBASE_t *adc, uint8_t group, adcData_t *data);

/**
 * @brief Function to get ADC digital voltage
 * 
 * @param adc Pointer to ADC Module
 *        - adcREG1: ADC1 module pointer
 *        - adcREG2: ADC2 module pointer
 * @param group ADC group to be converted
 * @param channel ADC channel to be converted
 * @param adcData Pointer to converted data
 * 
 * @return 1 if successful, 0 if not 
 */ 

static uint8_t adcDigitalVoltage(adcBASE_t *adc, uint8_t group, uint8_t channel, adcData_t *adcData);

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