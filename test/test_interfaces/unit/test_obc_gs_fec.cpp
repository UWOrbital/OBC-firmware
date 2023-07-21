#include "obc_gs_fec.h"
#include "obc_gs_errors.h"

#include <string.h>

#include <gtest/gtest.h>

TEST(TestAx25SendRecv, iFrameNoStuff) {
  packed_rs_packet_t encodedData = {0};
  uint8_t data[RS_DECODED_SIZE];
  memset(data, 0xFF, RS_DECODED_SIZE);
  initRs();
  ASSERT_EQ(rsEncode(data, &encodedData), 0);
  // flip some bits
  encodedData.data[0] ^= 0b10000001;
  encodeData.data[222] ^= 0b10100011;
  uint8_t decodedData[RS_DECODED_SIZE];
  ASSERT_EQ(rsDecode(encodedData, decodedData, RS_DECODED_SIZE), 0);
  ASSERT_EQ(memcmp(decodedData, data, RS_DECODED_SIZE), 0);
}
