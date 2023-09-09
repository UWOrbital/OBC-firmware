#include "vn100_parsing.h"
#include "vn100.h"

#include "obc_errors.h"

#include <gtest/gtest.h>

TEST(VN100_PARSING_SUITE, EXPECT_SERIAL_ERROR_CODE) {
  const char[] errorStringValid = "$VNERR,C";
  const char[] errorStringInvalid = "$VNERR,E"

      VN100_error_t error = 0;
  ASSERT_EQ(parsePacket(VN_YMR, errorStringValid, &error), OBC_ERR_CODE_VN100_RESPONSE_ERROR);
  ASSERT_EQ(error, INSUFFICIENT_BAUD_RATE);

  ASSERT_EQ(parsePacket(VN_YMR, errorStringInvalid, &error), OBC_ERR_CODE_VN100_PARSE_ERROR);
}
