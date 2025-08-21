#include "sun_utils.h"

bool equalsPositionData(const position_data_t* data1, const position_data_t* data2) {
  if (data1 == NULL && data2 == NULL) {
    return true;
  } else if ((data1 == NULL && data2 != NULL) || (data1 != NULL && data2 == NULL)) {
    return false;
  }

  return (data1->julianDate == data2->julianDate && data1->x == data2->x && data1->y == data2->y &&
          data1->z == data2->z);
}

bool closePositionData(const position_data_t data1, const position_data_t data2) {
  return doubleCloseDefault(data1.julianDate, data2.julianDate) && doubleCloseDefault(data1.x, data2.x) &&
         doubleCloseDefault(data1.y, data2.y) && doubleCloseDefault(data1.z, data2.z);
}

obc_error_code_t linearlyInterpolate(julian_date_t targetJulianDate, position_t point1, position_t point2,
                                     julian_date_t jd1, julian_date_t jd2, position_t* buffer) {
  if (jd1 == jd2 || buffer == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  sciPrintf("p1: %f | p2: %f | jd1: %lf | jd2: %lf \r\n", point1, point2, jd1, jd2);
  *buffer = point1 + ((point2 - point1) / (jd2 - jd1)) * (targetJulianDate - jd1);
  sciPrintf("interpolated: %f\r\n", *buffer);
  return OBC_ERR_CODE_SUCCESS;
}

// Close functions

bool doubleCloseDefault(double a, double b) { return doubleClose(a, b, RELATIVE_TOLERANCE); }

bool doubleClose(double a, double b, double relativeTolerance) {
  sciPrintf("doubleAbs(a - b): %lf\r\n", doubleAbs(a - b));
  sciPrintf("relativeTolerance * doubleMax(doubleAbs(a), doubleAbs(b): %lf", relativeTolerance * doubleMax(doubleAbs(a), doubleAbs(b)));
  return doubleAbs(a - b) <= relativeTolerance * doubleMax(doubleAbs(a), doubleAbs(b));
}
