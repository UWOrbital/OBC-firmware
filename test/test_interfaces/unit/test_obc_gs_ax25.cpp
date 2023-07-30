#include "obc_gs_ax25.h"
#include "obc_gs_fec.h"
#include "obc_gs_errors.h"

#include <string.h>

#include <gtest/gtest.h>

TEST(TestAx25SendRecv, iFrameNoStuff) {
  uint8_t telemData[RS_ENCODED_SIZE] = {0};

  unstuffed_ax25_i_frame_t unstuffedAx25Data = {0};
  ASSERT_EQ(ax25SendIFrame(telemData, RS_ENCODED_SIZE, &unstuffedAx25Data, &groundStationCallsign),
            OBC_GS_ERR_CODE_SUCCESS);

  packed_ax25_i_frame_t ax25Data = {0};
  ASSERT_EQ(ax25Stuff(unstuffedAx25Data.data, unstuffedAx25Data.length, ax25Data.data, &ax25Data.length),
            OBC_GS_ERR_CODE_SUCCESS);
  EXPECT_EQ(ax25Data.length, AX25_MINIMUM_I_FRAME_LEN);

  unstuffed_ax25_i_frame_t unstuffedPacket = {0};
  ASSERT_EQ(ax25Unstuff(ax25Data.data, ax25Data.length, unstuffedPacket.data, &unstuffedPacket.length),
            OBC_GS_ERR_CODE_SUCCESS);
  EXPECT_EQ(unstuffedPacket.length, AX25_MINIMUM_I_FRAME_LEN);

  ASSERT_EQ(ax25Recv(&unstuffedPacket), OBC_GS_ERR_CODE_SUCCESS);
  for (uint16_t i = 0; i < RS_ENCODED_SIZE; ++i) {
    EXPECT_EQ(telemData[i], unstuffedPacket.data[AX25_INFO_FIELD_POSITION + i]);
  }
}

TEST(TestAx25SendRecv, iFrameMaxStuff) {
  uint8_t telemData[RS_ENCODED_SIZE] = {0};
  memset(telemData, 0xFF, RS_ENCODED_SIZE);

  unstuffed_ax25_i_frame_t unstuffedAx25Data = {0};
  ASSERT_EQ(ax25SendIFrame(telemData, RS_ENCODED_SIZE, &unstuffedAx25Data, &groundStationCallsign),
            OBC_GS_ERR_CODE_SUCCESS);

  packed_ax25_i_frame_t ax25Data = {0};
  ASSERT_EQ(ax25Stuff(unstuffedAx25Data.data, unstuffedAx25Data.length, ax25Data.data, &ax25Data.length),
            OBC_GS_ERR_CODE_SUCCESS);
  EXPECT_TRUE(ax25Data.length > AX25_MINIMUM_I_FRAME_LEN && ax25Data.length < AX25_MAXIMUM_PKT_LEN);

  unstuffed_ax25_i_frame_t unstuffedPacket = {0};
  ASSERT_EQ(ax25Unstuff(ax25Data.data, ax25Data.length, unstuffedPacket.data, &unstuffedPacket.length),
            OBC_GS_ERR_CODE_SUCCESS);
  EXPECT_EQ(unstuffedPacket.length, AX25_MINIMUM_I_FRAME_LEN);

  ASSERT_EQ(ax25Recv(&unstuffedPacket), OBC_GS_ERR_CODE_SUCCESS);
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

  unstuffed_ax25_i_frame_t unstuffedAx25Data = {0};
  ASSERT_EQ(ax25SendIFrame(telemData, RS_ENCODED_SIZE, &unstuffedAx25Data, &groundStationCallsign),
            OBC_GS_ERR_CODE_SUCCESS);

  packed_ax25_i_frame_t ax25Data = {0};
  ASSERT_EQ(ax25Stuff(unstuffedAx25Data.data, unstuffedAx25Data.length, ax25Data.data, &ax25Data.length),
            OBC_GS_ERR_CODE_SUCCESS);

  EXPECT_TRUE(ax25Data.length > AX25_MINIMUM_I_FRAME_LEN && ax25Data.length < AX25_MAXIMUM_PKT_LEN);

  unstuffed_ax25_i_frame_t unstuffedPacket = {0};
  ASSERT_EQ(ax25Unstuff(ax25Data.data, ax25Data.length, unstuffedPacket.data, &unstuffedPacket.length),
            OBC_GS_ERR_CODE_SUCCESS);
  EXPECT_EQ(unstuffedPacket.length, AX25_MINIMUM_I_FRAME_LEN);

  ASSERT_EQ(ax25Recv(&unstuffedPacket), OBC_GS_ERR_CODE_SUCCESS);
  for (uint16_t i = 0; i < RS_ENCODED_SIZE; ++i) {
    EXPECT_EQ(telemData[i], unstuffedPacket.data[AX25_INFO_FIELD_POSITION + i]);
  }
}

TEST(TestAx25SendRecv, uFrameSendRecv) {
  packed_ax25_u_frame_t ax25Data = {0};
  uint8_t pollFinalBit = 1;

  ASSERT_EQ(ax25SendUFrame(&ax25Data, U_FRAME_CMD_ACK, pollFinalBit, &cubesatCallsign), OBC_GS_ERR_CODE_SUCCESS);

  unstuffed_ax25_i_frame_t unstuffedPacket = {0};
  ASSERT_EQ(ax25Unstuff(ax25Data.data, ax25Data.length, unstuffedPacket.data, &unstuffedPacket.length),
            OBC_GS_ERR_CODE_SUCCESS);
  ASSERT_EQ(ax25Recv(&unstuffedPacket), OBC_GS_ERR_CODE_SUCCESS);
}

TEST(TestAx25SendRecv, iFrameSendRecvFlagShare) {

  uint8_t telemDataFS[255] = {0};
  uint8_t unstuffedAx25DataFS[276] = {0};
  uint16_t axDataLen = {0};

  ASSERT_EQ(ax25SendIFrameWithFlagSharing(telemDataFS, RS_ENCODED_SIZE, unstuffedAx25DataFS, &axDataLen, &groundStationCallsign),
            OBC_GS_ERR_CODE_SUCCESS);
  ASSERT_EQ(axDataLen, 1);
}