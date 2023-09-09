#include "sun_utils.h"
#include <stdio.h>

int equalsPositionData(const position_data_t data1, const position_data_t data2) {
  return (data1.julianDate == data2.julianDate && data1.x == data2.x && data1.y == data2.y && data1.z == data2.z);
}

obc_error_code_t linearlyInterpolate(julian_date_t targetJulianDate, position_t point1, position_t point2,
                                     julian_date_t jd1, julian_date_t jd2, position_t *buffer) {
  if (jd1 == jd2 || buffer == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  *buffer = point1 + ((point1 - point2) / (jd1 - jd2)) * (targetJulianDate - jd1);
  return OBC_ERR_CODE_SUCCESS;
}

// Close functions

uint8_t doubleCloseDefault(double a, double b) { return doubleClose(a, b, RELATIVE_TOLERANCE); }

uint8_t doubleClose(double a, double b, double relativeTolerance) {
  return doubleAbs(a - b) <= relativeTolerance * doubleMax(doubleAbs(a), doubleAbs(b));
}
