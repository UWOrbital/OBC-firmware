#include "obc_gs_ax25.h"
#include "obc_gs_fec.h"
#include "obc_gs_aes128.h"
#include "obc_gs_errors.h"

#include <string.h>

#include <gtest/gtest.h>
#include <stdio.h>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iostream>

// TEST: See if it encode / decode basic data (random data)
// TEST: See if decode works with some bits flipped

TEST(TestEncodeDecode, encodeData) {
  uint8_t data[RS_DECODED_SIZE];
  uint32_t seed = 0x4A25C3;  // random number
  for (uint16_t i = 0; i < RS_DECODED_SIZE; ++i) {
    // Pseudorandom generation using a simple algorithm
    seed = (seed * 1103515245 + 12345) % (1 << 31);
    data[i] = (uint8_t)(seed & 0xFF);
  }

  packed_rs_packet_t fecData = {0};
  ASSERT_EQ(rsEncode(data, &fecData), OBC_GS_ERR_CODE_SUCCESS);

  setCurrentLinkDestCallSign((uint8_t *)"AKITO", CALLSIGN_LENGTH, DEFAULT_SSID);

  unstuffed_ax25_i_frame_t unstuffedAx25Data = {0};
  ASSERT_EQ(ax25SendIFrame((uint8_t *)&fecData.data, RS_ENCODED_SIZE, &unstuffedAx25Data), OBC_GS_ERR_CODE_SUCCESS);

  packed_ax25_i_frame_t ax25Data = {0};
  ASSERT_EQ(ax25Stuff(unstuffedAx25Data.data, unstuffedAx25Data.length, (uint8_t *)ax25Data.data, &ax25Data.length),
            OBC_GS_ERR_CODE_SUCCESS);

  std::fstream writeFile("test.bin", std::ios::binary | std::ios::out);
  ASSERT_EQ(writeFile.is_open(), true);
  writeFile.clear();
  writeFile.write((char *)(&ax25Data.data[0]), sizeof(uint8_t) * ax25Data.length);
  std::remove("encode.bin");
  std::rename("test.bin", "encode.bin");
  writeFile.close();
}
TEST(TestEncodeDecode, decodeData) {
  std::fstream readFile("encode_more.bin", std::ios::in | std::ios::binary);
  ASSERT_EQ(readFile.is_open(), true);

  packed_ax25_i_frame_t ax25Data = {.length = AX25_MINIMUM_I_FRAME_LEN + 1};
  readFile.read((char *)(&ax25Data.data[0]), sizeof(uint8_t) * ax25Data.length);

  unstuffed_ax25_i_frame_t unstuffedPacket = {0};
  ASSERT_EQ(ax25Unstuff(ax25Data.data, ax25Data.length, unstuffedPacket.data, &unstuffedPacket.length),
            OBC_GS_ERR_CODE_SUCCESS);

  u_frame_cmd_t command;
  ASSERT_EQ(ax25Recv(&unstuffedPacket, &command), OBC_GS_ERR_CODE_SUCCESS);

  readFile.close();
}
TEST(TestEncodeDecode, decodeErroneousData) {}
