#pragma once

#include "obc_errors.h"
#include <stdint.h>

typedef double julian_date_t;
typedef float position_t;

/**
 * @struct	position_data_t
 * @brief	Holds a single position data point
*/
typedef struct position_data_t {
    julian_date_t julianDate;
    position_t x;
    position_t y;
    position_t z;
} position_data_t;

int equalsDataPoint(const position_data_t a, const position_data_t b);

/**
 * @brief Calculates the value of the point based on the target JD using the point1 as the lower value in the linear 
 * interpolation
 * 
 * @attention requires jd1 is not equal to jd2 and buffer is a valid pointer
 * 
 * @param targetJulianDate The data for the julian date we want
 * @param point1 The 1st point we are using to linearly interpolate
 * @param point2 The 2nd point we are using to linearly interpolate
 * @param jd1 The 1st julian date we are using to linearly interpolate
 * @param jd2 The 1st julian date we are using to linearly interpolate
 * 
 * @return Returns the value of the point at the targetJulanDate liner interpolated
*/
obc_error_code_t linearlyInterpolate(julian_date_t targetJulianDate, 
                                            position_t point1, 
                                            position_t point2, 
                                            julian_date_t jd1, 
                                            julian_date_t jd2,
                                            position_t *buffer);
