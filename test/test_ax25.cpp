#include "ax25.h"
#include "obc_errors.h"

#include <string.h>

#include <gtest/gtest.h>

TEST(TestAx25SendRecv, iFrameNoStuff) {
  uint8_t telemData[RS_ENCODED_SIZE] = {0};
  packed_ax25_i_frame_t ax25Data = {0};
  ax25SendIFrame(telemData, RS_ENCODED_SIZE, &ax25Data, &groundStationCallsign);
  EXPECT_EQ(ax25Data.length, AX25_MINIMUM_I_FRAME_LEN);

  unstuffed_ax25_i_frame_t unstuffedPacket = {0};
  EXPECT_EQ(ax25Unstuff(ax25Data.data, ax25Data.length, unstuffedPacket.data, &unstuffedPacket.length), 0);
  EXPECT_EQ(unstuffedPacket.length, AX25_MINIMUM_I_FRAME_LEN);
  EXPECT_EQ(ax25Recv(&unstuffedPacket), 0);
  for (uint16_t i = 0; i < RS_ENCODED_SIZE; ++i) {
    EXPECT_EQ(telemData[i], unstuffedPacket.data[AX25_INFO_FIELD_POSITION + i]);
  }
}

TEST(TestAx25SendRecv, iFrameMaxStuff) {
  uint8_t telemData[RS_ENCODED_SIZE] = {0};
  memset(telemData, 0xFF, RS_ENCODED_SIZE);
  packed_ax25_i_frame_t ax25Data = {0};
  ax25SendIFrame(telemData, RS_ENCODED_SIZE, &ax25Data, &groundStationCallsign);

  EXPECT_TRUE(ax25Data.length > AX25_MINIMUM_I_FRAME_LEN && ax25Data.length < AX25_MAXIMUM_PKT_LEN);

  unstuffed_ax25_i_frame_t unstuffedPacket = {0};
  EXPECT_EQ(ax25Unstuff(ax25Data.data, ax25Data.length, unstuffedPacket.data, &unstuffedPacket.length), 0);
  EXPECT_EQ(unstuffedPacket.length, AX25_MINIMUM_I_FRAME_LEN);
  EXPECT_EQ(ax25Recv(&unstuffedPacket), 0);
  for (uint16_t i = 0; i < RS_ENCODED_SIZE; ++i) {
    EXPECT_EQ(telemData[i], unstuffedPacket.data[AX25_INFO_FIELD_POSITION + i]);
  }
}

TEST(TestAx25SendRecv, iFrameSomeStuff) {
  uint8_t telemData[RS_ENCODED_SIZE] = {0};
  uint32_t seed = 0xF72FA1;  // random number
  for (uint16_t i = 0; i < RS_ENCODED_SIZE; ++i) {
    // Pseudorandom generation using a simple algorithm
    seed = (seed * 1103515245 + 12345) % (1 << 31);
    telemData[i] = (uint8_t)(seed & 0xFF);
  }
  packed_ax25_i_frame_t ax25Data = {0};
  ax25SendIFrame(telemData, RS_ENCODED_SIZE, &ax25Data, &groundStationCallsign);

  EXPECT_TRUE(ax25Data.length > AX25_MINIMUM_I_FRAME_LEN && ax25Data.length < AX25_MAXIMUM_PKT_LEN);

  unstuffed_ax25_i_frame_t unstuffedPacket = {0};
  EXPECT_EQ(ax25Unstuff(ax25Data.data, ax25Data.length, unstuffedPacket.data, &unstuffedPacket.length), 0);
  EXPECT_EQ(unstuffedPacket.length, AX25_MINIMUM_I_FRAME_LEN);
  EXPECT_EQ(ax25Recv(&unstuffedPacket), 0);
  for (uint16_t i = 0; i < RS_ENCODED_SIZE; ++i) {
    EXPECT_EQ(telemData[i], unstuffedPacket.data[AX25_INFO_FIELD_POSITION + i]);
  }
}

TEST(TestAx25SendRecv, uFrameSendRecv) {
  packed_ax25_u_frame_t ax25Data = {0};
  uint8_t pollFinalBit = 1;
  EXPECT_EQ(ax25SendUFrame(&ax25Data, U_FRAME_CMD_ACK, pollFinalBit, &cubesatCallsign), 0);
  unstuffed_ax25_i_frame_t unstuffedPacket = {0};
  EXPECT_EQ(ax25Unstuff(ax25Data.data, ax25Data.length, unstuffedPacket.data, &unstuffedPacket.length), 0);
  EXPECT_EQ(ax25Recv(&unstuffedPacket), 0);
}
