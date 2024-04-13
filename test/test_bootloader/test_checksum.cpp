#include "bl_common.h"
#include <stdio.h>

#include <gtest/gtest.h>

TEST(ChecksumTest, CRC32Checks) {
  char string[] = "a";

  uint32_t result = 0xDEADBEEF;
  std::cout << result << std::endl;
}
