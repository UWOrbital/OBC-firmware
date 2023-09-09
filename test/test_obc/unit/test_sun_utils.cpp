#include "sun_utils.h"

#include "obc_errors.h"

#include <gtest/gtest.h>

TEST(TestSunUtils, equalsPositionDataTrue) {
  position_data_t data1 = {0, 1, 2, 3};
  position_data_t data2 = {0, 1, 2, 3};
  ASSERT_TRUE(equalsPositionData(data1, data2));
}

TEST(TestSunUtils, equalsPositionDataFalseX) {
  position_data_t data1 = {0, 1, 2, 3};
  position_data_t data2 = {0, 2, 2, 3};
  ASSERT_FALSE(equalsPositionData(data1, data2));
}

TEST(TestSunUtils, equalsPositionDataFalseY) {
  position_data_t data1 = {0, 1, 3, 3};
  position_data_t data2 = {0, 1, 2, 3};
  ASSERT_FALSE(equalsPositionData(data1, data2));
}

TEST(TestSunUtils, equalsPositionDataFalseZ) {
  position_data_t data1 = {0, 1, 2, 4};
  position_data_t data2 = {0, 1, 2, 3};
  ASSERT_FALSE(equalsPositionData(data1, data2));
}

TEST(TestSunUtils, equalsPositionDataFalseJD) {
  position_data_t data1 = {1, 1, 2, 3};
  position_data_t data2 = {0, 1, 2, 3};
  ASSERT_FALSE(equalsPositionData(data1, data2));
}

TEST(TestSunUtils, linearlyInterpolateFunc) {
  julian_date_t targetJD = 10;
  julian_date_t jd1 = 2;
  julian_date_t jd2 = 12;
  position_t p1 = 5;
  position_t p2 = 60;

  position_t buffer;
  ASSERT_EQ(linearlyInterpolate(targetJD, p1, p2, jd1, jd2, &buffer), OBC_ERR_CODE_SUCCESS);
  ASSERT_TRUE(buffer, 50);
  ASSERT_EQ(linearlyInterpolate(targetJD, p1, p2, jd1, jd2, nullptr), OBC_ERR_CODE_INVALID_ARG);
  ASSERT_EQ(linearlyInterpolate(targetJD, p1, p1, jd1, jd2, &buffer), OBC_ERR_CODE_INVALID_ARG);
}

TEST(TestSunUtils, doubleCloseDefaultTolerance) {
  double d1 = 1.5;
  double d2 = 1.5 + 1e-10;

  ASSERT_TRUE(doubleClose(d1, d2));

  d2 = 1.5 + 1e-8;
  ASSERT_FALSE(doubleClose(d1, d2));
}

TEST(TestSunUtils, doubleCloseDefaultTolerance) {
  double d1 = 1.5;
  double d2 = 1.5 + 1e-10;
  double tolerance = 1e-5;

  ASSERT_TRUE(doubleClose(d1, d2, tolerance));

  d2 = 1.5 + 1e-2;
  ASSERT_FALSE(doubleClose(d1, d2));
}
