#include "sun_utils.h"

#include "obc_errors.h"

#include <gtest/gtest.h>

TEST(TestSunUtils, equalsPositionDataTrue) {
  position_data_t data1 = {0, 1, 2, 3};
  position_data_t data2 = {0, 1, 2, 3};
  EXPECT_TRUE(equalsPositionData(data1, data2));
}

TEST(TestSunUtils, equalsPositionDataFalseX) {
  position_data_t data1 = {0, 1, 2, 3};
  position_data_t data2 = {0, 2, 2, 3};
  EXPECT_FALSE(equalsPositionData(data1, data2));
}

TEST(TestSunUtils, equalsPositionDataFalseY) {
  position_data_t data1 = {0, 1, 3, 3};
  position_data_t data2 = {0, 1, 2, 3};
  EXPECT_FALSE(equalsPositionData(data1, data2));
}

TEST(TestSunUtils, equalsPositionDataFalseZ) {
  position_data_t data1 = {0, 1, 2, 4};
  position_data_t data2 = {0, 1, 2, 3};
  EXPECT_FALSE(equalsPositionData(data1, data2));
}

TEST(TestSunUtils, equalsPositionDataFalseJD) {
  position_data_t data1 = {1, 1, 2, 3};
  position_data_t data2 = {0, 1, 2, 3};
  EXPECT_FALSE(equalsPositionData(data1, data2));
}

TEST(TestSunUtils, linearlyInterpolateFunc) {
  julian_date_t targetJD = 10;
  julian_date_t jd1 = 2;
  julian_date_t jd2 = 12;
  position_t p1 = 10;
  position_t p2 = 60;

  position_t buffer;
  EXPECT_EQ(linearlyInterpolate(targetJD, p1, p2, jd1, jd2, &buffer), OBC_ERR_CODE_SUCCESS);
  EXPECT_EQ(buffer, 50);
  EXPECT_EQ(linearlyInterpolate(targetJD, p1, p2, jd1, jd2, nullptr), OBC_ERR_CODE_INVALID_ARG);
  EXPECT_EQ(linearlyInterpolate(targetJD, p1, p2, jd1, jd1, &buffer), OBC_ERR_CODE_INVALID_ARG);  // Same JD
}

TEST(TestSunUtils, doubleCloseDefaultTolerance) {
  double d1 = 1.5;
  double d2 = 1.5 + 1e-10;

  EXPECT_TRUE(doubleCloseDefault(d1, d2));

  d2 = 1.5 + 1e-8;
  EXPECT_FALSE(doubleCloseDefault(d1, d2));
}

TEST(TestSunUtils, doubleCloseFunc) {
  double d1 = 1.5;
  double d2 = 1.5 + 1e-10;
  double tolerance = 1e-5;

  EXPECT_TRUE(doubleClose(d1, d2, tolerance));

  d2 = 1.5 + 1e-2;
  EXPECT_FALSE(doubleClose(d1, d2, tolerance));
}
