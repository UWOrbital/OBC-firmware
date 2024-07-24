#include "obc_gs_fec.h"
#include "obc_gs_errors.h"

#include <string.h>

#include <gtest/gtest.h>

TEST(TestFecEncodeDecode, EncodeDecodeZeroData) {
  packed_rs_packet_t encodedData = {0};
  uint8_t data[RS_DECODED_SIZE] = {0};

  memset(data, 0xFF, RS_DECODED_SIZE);

  ASSERT_EQ(rsEncode(data, &encodedData), OBC_GS_ERR_CODE_SUCCESS);

  // Corrupt the data
  encodedData.data[0] ^= 0b10000001;
  encodedData.data[222] ^= 0b10100011;

  uint8_t decodedData[RS_DECODED_SIZE] = {0};
  ASSERT_EQ(rsDecode(&encodedData, decodedData, RS_DECODED_SIZE), OBC_GS_ERR_CODE_SUCCESS);

  EXPECT_EQ(memcmp(decodedData, data, RS_DECODED_SIZE), 0);
}

TEST(TestFecEncodeDecode, EncodeDecodeNonZero) {
  packed_rs_packet_t encodedData = {0};
  uint8_t data[RS_DECODED_SIZE] = {0};

  uint32_t seed = 0x7A21C3;  // random number
  for (uint16_t i = 0; i < RS_ENCODED_SIZE; ++i) {
    // Pseudorandom generation using a simple algorithm
    seed = (seed * 1103515245 + 12345) % (1 << 31);
    data[i] = (uint8_t)(seed & 0xFF);
  }

  ASSERT_EQ(rsEncode(data, &encodedData), OBC_GS_ERR_CODE_SUCCESS);

  // Corrupt the data
  encodedData.data[0] ^= 0b10000001;
  encodedData.data[222] ^= 0b10100011;

  uint8_t decodedData[RS_DECODED_SIZE] = {0};
  ASSERT_EQ(rsDecode(&encodedData, decodedData, RS_DECODED_SIZE), OBC_GS_ERR_CODE_SUCCESS);
  EXPECT_EQ(memcmp(decodedData, data, RS_DECODED_SIZE), 0);
}

// Attempt to replicate rs_test.c as closely as possible
TEST(TestFecEncodeDecode, EncodeDecodeHelloWorld) {
  packed_rs_packet_t encodedData = {0};
  packed_telem_packet_t telemData = {.data = {'-'}};

  const char *testData = "Hello world!";
  const size_t testDataLen = strlen(testData);
  for (int i = 0; i < testDataLen; i++) {
    telemData.data[i] = (uint8_t)testData[i];
  }

  ASSERT_EQ(rsEncode(telemData.data, &encodedData), OBC_GS_ERR_CODE_SUCCESS);

  uint8_t decodedData[RS_DECODED_SIZE] = {0};
  ASSERT_EQ(rsDecode(&encodedData, decodedData, RS_DECODED_SIZE), OBC_GS_ERR_CODE_SUCCESS);
  EXPECT_EQ(memcmp(decodedData, telemData.data, testDataLen), 0);
}
