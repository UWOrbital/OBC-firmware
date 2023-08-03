#include "obc_gs_ax25.h"
#include "obc_gs_ax25.c"
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

//TEST(TestAx25SendRecv, uFrameSendRecv) {
//  packed_ax25_u_frame_t ax25Data = {0};
//  uint8_t pollFinalBit = 1;
//
//  ASSERT_EQ(ax25SendUFrame(&ax25Data, U_FRAME_CMD_ACK, pollFinalBit, &cubesatCallsign), OBC_GS_ERR_CODE_SUCCESS);
//
//  unstuffed_ax25_i_frame_t unstuffedPacket = {0};
//  ASSERT_EQ(ax25Unstuff(ax25Data.data, ax25Data.length, unstuffedPacket.data, &unstuffedPacket.length),
//            OBC_GS_ERR_CODE_SUCCESS);
//  ASSERT_EQ(ax25Recv(&unstuffedPacket), OBC_GS_ERR_CODE_SUCCESS);
//}

TEST(TestAx25SendRecv, iFrameSendRecvFlagShare) {
  // uint8_t *telemData, uint8_t telemDataLen, unstuffed_ax25_i_frame_t *ax25Data,
  //                                   const ax25_addr_t *destAddress
  unstuffed_ax25_i_frame_t ax25Data = {0};
  uint8_t telemData[AX25_INFO_BYTES] = {0};
  uint8_t telemDataLen = AX25_INFO_BYTES;
  const ax25_addr_t *destAddress = &groundStationCallsign;

  memset(ax25Data.data, 0, AX25_MINIMUM_I_FRAME_LEN);
  ax25Data.length = AX25_MINIMUM_I_FRAME_LEN;
  ax25Data.data[0] = AX25_FLAG;
  memcpy(ax25Data.data + AX25_DEST_ADDR_POSITION, destAddress->data, AX25_DEST_ADDR_BYTES);
  uint8_t srcAddress[AX25_SRC_ADDR_BYTES] = SRC_CALLSIGN;
  memcpy(ax25Data.data + AX25_SRC_ADDR_POSITION, srcAddress, AX25_SRC_ADDR_BYTES);
  ax25Data.data[AX25_CONTROL_BYTES_POSITION] = 0;
  ax25Data.data[AX25_CONTROL_BYTES_POSITION + 1] = 0;
  ax25Data.data[AX25_MOD128_PID_POSITION] = AX25_PID;
  memcpy(ax25Data.data + AX25_INFO_FIELD_POSITION, telemData, telemDataLen);
  obc_gs_error_code_t errCode;
  uint16_t fcs;
  errCode = fcsCalculate(ax25Data.data, AX25_MINIMUM_I_FRAME_LEN, &fcs);
  ax25Data.data[AX25_I_FRAME_FCS_POSITION] = (uint8_t)(fcs >> 8);
  ax25Data.data[AX25_I_FRAME_FCS_POSITION + 1] = (uint8_t)(fcs & 0xFF);
  ax25Data.data[ax25Data.length - 1] = AX25_FLAG;
  ax25Data.data[0] = AX25_FLAG;

  ASSERT_EQ(ax25Data.data[0], AX25_FLAG);

  uint8_t secondAx25Data[(2 * AX25_MINIMUM_I_FRAME_LEN) + 1];
  uint8_t secondTelemData[2 * AX25_INFO_BYTES] = {0};
  uint16_t secondTelemDataLen = 2 * AX25_INFO_BYTES;

  memset(secondAx25Data, 0, (2 * AX25_MINIMUM_I_FRAME_LEN_SHARE_FLAG) + 1);
  secondAx25Data[0] = AX25_FLAG;
  memcpy(secondAx25Data + AX25_DEST_ADDR_POSITION, destAddress->data, AX25_DEST_ADDR_BYTES);
  memcpy(secondAx25Data + AX25_SRC_ADDR_POSITION, srcAddress, AX25_SRC_ADDR_BYTES);
  secondAx25Data[AX25_CONTROL_BYTES_POSITION] = 0;
  secondAx25Data[AX25_CONTROL_BYTES_POSITION + 1] = 0;
  secondAx25Data[AX25_MOD128_PID_POSITION] = AX25_PID;
  memcpy(secondAx25Data + AX25_INFO_FIELD_POSITION, telemData, telemDataLen);
  obc_gs_error_code_t errCodeOne;
  uint16_t fcsOne;
  errCodeOne = fcsCalculate(secondAx25Data, AX25_MINIMUM_I_FRAME_LEN, &fcsOne);
  secondAx25Data[AX25_I_FRAME_FCS_POSITION] = (uint8_t)(fcs >> 8);
  secondAx25Data[AX25_I_FRAME_FCS_POSITION + 1] = (uint8_t)(fcs & 0xFF);

  secondAx25Data[(2 * AX25_MINIMUM_I_FRAME_LEN_SHARE_FLAG)] = AX25_FLAG;

  memcpy(secondAx25Data + AX25_MINIMUM_I_FRAME_LEN + AX25_DEST_ADDR_POSITION, destAddress->data, AX25_DEST_ADDR_BYTES);
  memcpy(secondAx25Data + AX25_MINIMUM_I_FRAME_LEN + AX25_SRC_ADDR_POSITION, srcAddress, AX25_SRC_ADDR_BYTES);
  secondAx25Data[AX25_MINIMUM_I_FRAME_LEN + AX25_CONTROL_BYTES_POSITION] = 0;
  secondAx25Data[AX25_MINIMUM_I_FRAME_LEN + AX25_CONTROL_BYTES_POSITION + 1] = 0;
  secondAx25Data[AX25_MINIMUM_I_FRAME_LEN + AX25_MOD128_PID_POSITION] = AX25_PID;
  memcpy(secondAx25Data + AX25_MINIMUM_I_FRAME_LEN + AX25_INFO_FIELD_POSITION, telemData, telemDataLen);
  obc_gs_error_code_t errCodeTwo;
  uint16_t fcsTwo;
  errCodeTwo = fcsCalculate(secondAx25Data + AX25_MINIMUM_I_FRAME_LEN - 1, AX25_MINIMUM_I_FRAME_LEN, &fcsTwo);
  secondAx25Data[AX25_MINIMUM_I_FRAME_LEN + AX25_I_FRAME_FCS_POSITION] = (uint8_t)(fcs >> 8);
  secondAx25Data[AX25_MINIMUM_I_FRAME_LEN + AX25_I_FRAME_FCS_POSITION + 1] = (uint8_t)(fcs & 0xFF);
  
}