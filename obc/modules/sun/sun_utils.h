// This file contains utility functions that MUST ONLY be used in the
// sun position project files
// None of the operations can be assumed to be atomic,
// it is the responsibility of the caller to setup the appropriate locks
#pragma once

#include "obc_errors.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define ADCS_INVALID_JULIAN_DATE 0U
#define RELATIVE_TOLERANCE 1e-9f

#ifdef __cplusplus
extern "C" {
#endif

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

/**
 * @brief Returns true if the data points 1 and 2 have the same coordinates and julian date
 */
bool equalsPositionData(const position_data_t data1, const position_data_t data2);

/**
 * @brief Returns true if the data points 1 and 2 have the close coordinates and julian date
 */
bool closePositionData(const position_data_t data1, const position_data_t data2);

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
obc_error_code_t linearlyInterpolate(julian_date_t targetJulianDate, position_t point1, position_t point2,
                                     julian_date_t jd1, julian_date_t jd2, position_t *buffer);

// These are place here as doubles are 64-bit thus these operations may not be atomic

/**
 * @brief returns the maximum of 2 double numbers
 *
 * @param a - first double number to compare
 * @param b - second double number to compare
 * @return the maximum of the 2 numbers
 */
static inline double doubleMax(double a, double b) { return (a) > (b) ? (a) : (b); }

/**
 * @brief returns the absolute value of a double number
 * @param num - the double number to get the absolute value of
 * @return the absolute value of the double number
 */
static inline double doubleAbs(double num) { return (num) < 0 ? -(num) : (num); }

/**
 * @brief Checks whether the numbers a and b are close within the default tolerance
 * interval. Formula is the same as the python math.isclose() function except that absolute tolerance is not used
 * (abs (a-b) <= RELATIVE_TOLERANCE * max(abs(a), abs(b)))
 * @param a: first double number to check
 * @param b: second double number to check
 * @warning This operation is not atomic. It is the responsibility of the caller to setup the appropriate locks
 */
bool doubleCloseDefault(double a, double b);

/**
 * @brief Checks whether the numbers a and b are close within the given
 * interval. Formula is the same as the python math.isclose() function except that absolute tolerance is not used
 * (abs (a-b) <= relativeTolerance * max(abs(a), abs(b)))
 * @param a: first double number to check
 * @param b: second double number to check
 * @param relativeTolerance: Relative tolerance decimal
 * @warning This operation is not atomic. It is the responsibility of the caller to setup the appropriate locks
 */
bool doubleClose(double a, double b, double relativeTolerance);

#ifdef __cplusplus
}
#endif
