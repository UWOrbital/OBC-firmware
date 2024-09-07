#include "obc_gs_uplink_flow.h"
#include "obc_gs_ax25.h"
#include "obc_gs_command_data.h"
#include "obc_gs_command_pack.h"
#include "obc_gs_command_id.h"

#include <gtest/gtest.h>
#include <string.h>

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

  setCurrentLinkDestAddress(&groundStationCallsign);
  ASSERT_EQ(uplinkEncodePacket(&input, &ax25Data, aesKey), OBC_GS_ERR_CODE_SUCCESS);
  uplink_flow_packet_t output = {.data = {0}, .type = UPLINK_FLOW_DECODED_DATA};
  ASSERT_EQ(uplinkDecodePacket(&ax25Data, &output), OBC_GS_ERR_CODE_SUCCESS);
  EXPECT_EQ(memcmp(input.data, output.data, AES_DECRYPTED_SIZE), 0);
}

TEST(TestInterfaceUplinkFlow, PingCmd) {
  const uint8_t TEMP_STATIC_KEY[AES_KEY_SIZE] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                                 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
  ASSERT_EQ(initializeAesCtx(TEMP_STATIC_KEY), OBC_GS_ERR_CODE_SUCCESS);
  // Pack ping cmd
  cmd_msg_t cmdMsg = {.isTimeTagged = false, .id = CMD_PING};
  uint32_t cmdPacketOffset = 0;
  uint8_t packedSingleCmdSize = 0;
  uint8_t packedSingleCmd[MAX_CMD_MSG_SIZE] = {0};
  ASSERT_EQ(packCmdMsg(packedSingleCmd, &cmdPacketOffset, &cmdMsg, &packedSingleCmdSize), OBC_GS_ERR_CODE_SUCCESS);

  // Setup for packet encode
  setCurrentLinkDestAddress(&groundStationCallsign);
  uplink_flow_packet_t packet = {.data = {0}, .type = UPLINK_FLOW_DECODED_DATA};
  memcpy(packet.data, packedSingleCmd,
         packedSingleCmdSize < AES_DECRYPTED_SIZE ? packedSingleCmdSize : AES_DECRYPTED_SIZE);

  // Encode
  packed_ax25_i_frame_t ax25Data = {0};
  ASSERT_EQ(uplinkEncodePacket(&packet, &ax25Data, TEMP_STATIC_KEY), OBC_GS_ERR_CODE_SUCCESS);
  ASSERT_EQ(packet.type, UPLINK_FLOW_DECODED_DATA);

  // Decode packet
  uplink_flow_packet_t output = {.data = {0}, .type = UPLINK_FLOW_DECODED_DATA};
  ASSERT_EQ(uplinkDecodePacket(&ax25Data, &output), OBC_GS_ERR_CODE_SUCCESS);

  EXPECT_EQ(memcmp(packet.data, output.data, AES_DECRYPTED_SIZE), 0);
}
