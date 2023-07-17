#pragma once

#include "obc_errors.h"

/**
 * @brief powers on the max5460leuk DAC and sets it to output a certain voltage
 *
 * @param analogVoltsOutput the value in volts that will be outputted by the DAC
 *
 * @return obc_error_code_t - whether it was successful or not
 */
obc_error_code_t max5360WriteVoltage(float analogVoltsOutput);

/**
 * @brief powers off the max5360 and stops it from outputting anything
 *
 * @return obc_error_code_t whether the power off was successful or not
 */
obc_error_code_t max5360PowerOff(void);
