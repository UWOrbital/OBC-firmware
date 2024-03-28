#include "bl_common.h"
#include <stdio.h>

#include <gtest/gtest.h>

TEST(ChecksumTest, CRC32Checks) {
  char string[] = "a";

  uint32_t result = computeCRC32Checksum((uint8_t*)string, sizeof(string) / sizeof(string[0]));
  std::cout << result << std::endl;
}
