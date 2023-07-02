#include "obc_time_utils.h"
#include "obc_errors.h"

#include <gtest/gtest.h>

TEST(TestObcTimeUtils, ValidDatetimeToUnix) {
  rtc_date_time_t datetime = {.date =
                                  {
                                      .date = 10,
                                      .month = 2,
                                      .year = 23,
                                  },
                              .time = {.hours = 5, .minutes = 20, .seconds = 59}};

  const uint32_t expectedUnix = 1676006459;

  uint32_t unixTime;
  ASSERT_EQ(datetimeToUnix(&datetime, &unixTime), OBC_ERR_CODE_SUCCESS);

  ASSERT_EQ(unixTime, expectedUnix);
}

TEST(TestObcTimeUtils, ValidUnixToDatetime) {
  const uint32_t unixTime = 1676006459;

  rtc_date_time_t datetime = {.date =
                                  {
                                      .date = 10,
                                      .month = 2,
                                      .year = 23,
                                  },
                              .time = {.hours = 5, .minutes = 20, .seconds = 59}};

  ASSERT_EQ(unixToDatetime(unixTime, &datetime), OBC_ERR_CODE_SUCCESS);

  ASSERT_EQ(datetime.date.date, 10);
  ASSERT_EQ(datetime.date.month, 2);
  ASSERT_EQ(datetime.date.year, 23);

  ASSERT_EQ(datetime.time.hours, 5);
  ASSERT_EQ(datetime.time.minutes, 20);
  ASSERT_EQ(datetime.time.seconds, 59);
}
