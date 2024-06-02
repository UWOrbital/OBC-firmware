#include "uplink_flow.h"

#include <gtest/gtest.h>
#include <cstring>

TEST(TestInterfaceUplinkFlow, InvalidArgs) {
  packed_ax25_i_frame_t ax25Data = {0};
  uplink_flow_packet_t command = {.data = {0}, .type = UPLINK_FLOW_DECODED_DATA};

  EXPECT_EQ(uplinkDecodePacket(nullptr, &command), OBC_GS_ERR_CODE_INVALID_ARG);
  EXPECT_EQ(uplinkDecodePacket(&ax25Data, &command), OBC_GS_ERR_CODE_INVALID_ARG);
  EXPECT_EQ(uplinkDecodePacket(&ax25Data, &command), OBC_GS_ERR_CODE_INVALID_ARG);
  EXPECT_EQ(uplinkDecodePacket(&ax25Data, nullptr), OBC_GS_ERR_CODE_INVALID_ARG);
}

TEST(TestInterfaceUplinkFlow, Data) {
  packed_ax25_i_frame_t ax25Data = {0};
  uplink_flow_packet_t input = {.data = {1, 2, 3, 4, 5, 6, 7, 8}, .type = UPLINK_FLOW_DECODED_DATA};
  uint8_t aesKey[AES_KEY_SIZE] = {0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8};

  ASSERT_EQ(uplinkEncodePacket(&input, &ax25Data, aesKey), OBC_GS_ERR_CODE_SUCCESS);

  uplink_flow_packet_t output = {.data = {0}, .type = UPLINK_FLOW_DECODED_DATA};
  ASSERT_EQ(uplinkDecodePacket(&ax25Data, &output), OBC_GS_ERR_CODE_SUCCESS);

  EXPECT_EQ(memcmp(input.data, output.data, AES_DECRYPTED_SIZE), 0);
}
