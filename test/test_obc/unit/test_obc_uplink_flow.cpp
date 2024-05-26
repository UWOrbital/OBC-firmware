#include "uplink_flow.h"

#include <gtest/gtest.h>

TEST(TestObcUplinkFlow, UplinkDecodePacket) {
  packed_ax25_i_frame_t ax25Data = {0};
  packed_rs_packet_t rsData = {0};
  aes_data_t aesData = {0};
  uplink_flow_packet_t command = {0};

  EXPECT_EQ(uplinkDecodePacket(nullptr, &rsData, &aesData, &command), OBC_ERR_CODE_INVALID_ARG);
  EXPECT_EQ(uplinkDecodePacket(&ax25Data, nullptr, &aesData, &command), OBC_ERR_CODE_INVALID_ARG);
  EXPECT_EQ(uplinkDecodePacket(&ax25Data, &rsData, nullptr, &command), OBC_ERR_CODE_INVALID_ARG);
  EXPECT_EQ(uplinkDecodePacket(&ax25Data, &rsData, &aesData, nullptr), OBC_ERR_CODE_INVALID_ARG);
}
