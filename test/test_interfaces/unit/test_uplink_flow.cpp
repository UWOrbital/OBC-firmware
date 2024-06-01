#include "uplink_flow.h"

#include <gtest/gtest.h>

TEST(TestObcUplinkFlow, InvalidArgs) {
  packed_ax25_i_frame_t ax25Data = {0};
  uplink_flow_packet_t command = {.data = {0}, .type = UPLINK_FLOW_DECODED_DATA};

  EXPECT_EQ(uplinkDecodePacket(nullptr, &command), OBC_GS_ERR_CODE_INVALID_ARG);
  EXPECT_EQ(uplinkDecodePacket(&ax25Data, &command), OBC_GS_ERR_CODE_INVALID_ARG);
  EXPECT_EQ(uplinkDecodePacket(&ax25Data, &command), OBC_GS_ERR_CODE_INVALID_ARG);
  EXPECT_EQ(uplinkDecodePacket(&ax25Data, nullptr), OBC_GS_ERR_CODE_INVALID_ARG);
}
