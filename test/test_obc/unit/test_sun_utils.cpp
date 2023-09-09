#include "sun_utils.h"

#include "obc_errors.h"

#include <gtest/gtest.h>

TEST(TestSunUtils, equalsPositionDataTrue) {
  position_data_t data1 = {0, 1, 2, 3};
  position_data_t data2 = {0, 1, 2, 3};
  ASSERT_EQ(equalsDataPoint(data1, data2), 1);
}

TEST(TestSunUtils, equalsPositionDataFalseX) {
  position_data_t data1 = {0, 1, 2, 3};
  position_data_t data2 = {0, 2, 2, 3};
  ASSERT_EQ(equalsDataPoint(data1, data2), 0);
}

TEST(TestSunUtils, equalsPositionDataFalseY) {
  position_data_t data1 = {0, 1, 3, 3};
  position_data_t data2 = {0, 1, 2, 3};
  ASSERT_EQ(equalsDataPoint(data1, data2), 0);
}

TEST(TestSunUtils, equalsPositionDataFalseZ) {
  position_data_t data1 = {0, 1, 2, 4};
  position_data_t data2 = {0, 1, 2, 3};
  ASSERT_EQ(equalsDataPoint(data1, data2), 0);
}

TEST(TestSunUtils, equalsPositionDataFalseJD) {
  position_data_t data1 = {1, 1, 2, 3};
  position_data_t data2 = {0, 1, 2, 3};
  ASSERT_EQ(equalsDataPoint(data1, data2), 0);
}
