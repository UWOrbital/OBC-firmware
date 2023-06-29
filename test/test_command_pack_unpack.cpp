#include "command_pack.h"
#include "command_unpack.h"
#include "command_data.h"
#include "command_id.h"

#include <gtest/gtest.h>

// CMD_EXEC_OBC_RESET
TEST(TestCommandPackUnpack, ValidCmdExecObcResetPackUnpack) {
  cmd_msg_t cmdMsg = {0};
  cmdMsg.id = CMD_EXEC_OBC_RESET;

  uint8_t buff[MAX_CMD_MSG_SIZE] = {0};
  uint32_t packOffset = 0;
  uint8_t numPacked = 0;
  packCmdMsg(buff, &packOffset, &cmdMsg, &numPacked);

  cmd_msg_t unpackedCmdMsg = {0};
  uint32_t unpackOffset = 0;
  unpackCmdMsg(buff, &unpackOffset, &unpackedCmdMsg);

  ASSERT_EQ(packOffset, unpackOffset);
  ASSERT_EQ(cmdMsg.id, unpackedCmdMsg.id);
}

// CMD_RTC_SYNC
TEST(TestCommandPackUnpack, ValidCmdRtcSyncPackUnpack) {
  cmd_msg_t cmdMsg = {0};
  cmdMsg.id = CMD_RTC_SYNC;
  cmdMsg.rtcSync.unixTime = 0x12345678;

  uint8_t buff[MAX_CMD_MSG_SIZE] = {0};
  uint32_t packOffset = 0;
  uint8_t numPacked = 0;
  packCmdMsg(buff, &packOffset, &cmdMsg, &numPacked);

  cmd_msg_t unpackedCmdMsg = {0};
  uint32_t unpackOffset = 0;
  unpackCmdMsg(buff, &unpackOffset, &unpackedCmdMsg);

  ASSERT_EQ(packOffset, unpackOffset);
  ASSERT_EQ(cmdMsg.id, unpackedCmdMsg.id);
  ASSERT_EQ(cmdMsg.rtcSync.unixTime, unpackedCmdMsg.rtcSync.unixTime);
}

// CMD_DOWNLINK_LOGS_NEXT_PASS
TEST(TestCommandPackUnpack, ValidCmdDownlinkLogsNextPassPackUnpack) {
  cmd_msg_t cmdMsg = {0};
  cmdMsg.id = CMD_DOWNLINK_LOGS_NEXT_PASS;
  cmdMsg.downlinkLogsNextPass.logLevel = LOG_DEBUG;

  uint8_t buff[MAX_CMD_MSG_SIZE] = {0};
  uint32_t packOffset = 0;
  uint8_t numPacked = 0;
  packCmdMsg(buff, &packOffset, &cmdMsg, &numPacked);

  cmd_msg_t unpackedCmdMsg = {0};
  uint32_t unpackOffset = 0;
  unpackCmdMsg(buff, &unpackOffset, &unpackedCmdMsg);

  ASSERT_EQ(packOffset, unpackOffset);
  ASSERT_EQ(cmdMsg.id, unpackedCmdMsg.id);
  ASSERT_EQ(cmdMsg.downlinkLogsNextPass.logLevel, unpackedCmdMsg.downlinkLogsNextPass.logLevel);
}

// CMD_MICRO_SD_FORMAT
TEST(TestCommandPackUnpack, ValidCmdMicroSdFormatPackUnpack) {
  cmd_msg_t cmdMsg = {0};
  cmdMsg.id = CMD_MICRO_SD_FORMAT;

  uint8_t buff[MAX_CMD_MSG_SIZE] = {0};
  uint32_t packOffset = 0;
  uint8_t numPacked = 0;
  packCmdMsg(buff, &packOffset, &cmdMsg, &numPacked);

  cmd_msg_t unpackedCmdMsg = {0};
  uint32_t unpackOffset = 0;
  unpackCmdMsg(buff, &unpackOffset, &unpackedCmdMsg);

  ASSERT_EQ(packOffset, unpackOffset);
  ASSERT_EQ(cmdMsg.id, unpackedCmdMsg.id);
}

// CMD_PING
TEST(TestCommandPackUnpack, ValidCmdPingPackUnpack) {
  cmd_msg_t cmdMsg = {0};
  cmdMsg.id = CMD_PING;

  uint8_t buff[MAX_CMD_MSG_SIZE] = {0};
  uint32_t packOffset = 0;
  uint8_t numPacked = 0;
  packCmdMsg(buff, &packOffset, &cmdMsg, &numPacked);

  cmd_msg_t unpackedCmdMsg = {0};
  uint32_t unpackOffset = 0;
  unpackCmdMsg(buff, &unpackOffset, &unpackedCmdMsg);

  ASSERT_EQ(packOffset, unpackOffset);
  ASSERT_EQ(cmdMsg.id, unpackedCmdMsg.id);
}

// CMD_DOWNLINK_TELEM
TEST(TestCommandPackUnpack, ValidCmdDownlinkTelemPackUnpack) {
  cmd_msg_t cmdMsg = {0};
  cmdMsg.id = CMD_DOWNLINK_TELEM;

  uint8_t buff[MAX_CMD_MSG_SIZE] = {0};
  uint32_t packOffset = 0;
  uint8_t numPacked = 0;
  packCmdMsg(buff, &packOffset, &cmdMsg, &numPacked);

  cmd_msg_t unpackedCmdMsg = {0};
  uint32_t unpackOffset = 0;
  unpackCmdMsg(buff, &unpackOffset, &unpackedCmdMsg);

  ASSERT_EQ(packOffset, unpackOffset);
  ASSERT_EQ(cmdMsg.id, unpackedCmdMsg.id);
}
