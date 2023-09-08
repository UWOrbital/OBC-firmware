#pragma once

#include "sun_utils.h"
#include "sun_mgr.h"

#include "obc_errors.h"
#include "obc_logging.h"

#include <stdint.h>

/**
 * @brief	Initializes the sun position module
 * @todo Initialize file 
*/
void initSunPosition(void);

/**
 * @brief Gets the sun position at the given julian date
 * @attention jd must be greater than 0
 * @attention buffer must be a valid pointer
 * @param jd The julian date (or close to it) to get the sun position at
 * @param buffer The buffer to store the sun position in
 * 
 * @details May modify the file used for storing the data points
*/
obc_error_code_t sunPositionGet(julian_date_t jd, position_data_t *buffer);

/**
 * @brief Copies the next data point from the manager into the buffer and shifts the manager over
 * @attention buffer must be a valid pointer
 * @param buffer The buffer to store the next data point in
 * @details May modify the file and manager used for storing the data points
*/
obc_error_code_t sunPositionNext(position_data_t *buffer);

/**
 * @brief Shifts all the data points of the manager to start at the given julian date
 * @param jd The julian date to shift the data points to start at (the manager will choose the 
 * closest julian date stored in the file)
 * @attention jd must be greater than 0 and less than largest julian date that has the length of the data
 * manager number of points after it (including this julian date) 
*/
obc_error_code_t sunPositionShiftTo(julian_date_t jd);

