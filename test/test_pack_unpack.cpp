#include "obc_unpack_utils.h"
#include "obc_pack_utils.h"

#include <gtest/gtest.h>

TEST(TestPackAndUnpack, ValidUint8PackUnpack) {
  uint8_t val = 0x12;
  uint8_t buf[sizeof(val)];

  size_t len = 0;
  packUint8(val, buf, &len);
  EXPECT_EQ(len, sizeof(uint8_t));

  len = 0;
  uint8_t unpackedVal = unpackUint8(buf, (uint32_t *)&len);

  EXPECT_EQ(len, sizeof(uint8_t));
  EXPECT_EQ(val, unpackedVal);
}
