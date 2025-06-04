
#include "obc_gs_commands_response_pack.h"
#include "obc_gs_commands_response_unpack.h"
#include "obc_gs_commands_response.h"
#include "data_unpack_utils.h"
#include "obc_gs_errors.h"

#include <iostream>
#include <gtest/gtest.h>
#include <stdbool.h>
#include <stdio.h>
#include <iostream>

TEST(pack_unpack_command_responses, packResponse) {
  cmd_unpacked_response_t unpackedResponse = {
      .errCode = CMD_RESPONSE_SUCCESS, .cmdId = CMD_EXEC_OBC_RESET, .obcResetResponse = {.data1 = 0.02, .data2 = 2}};

  uint8_t buffer[CMD_RESPONSE_MAX_PACKED_SIZE] = {0};
  obc_gs_error_code_t errCode = packCommandResponse(&unpackedResponse, buffer);
  ASSERT_EQ(errCode, OBC_GS_ERR_CODE_SUCCESS);

  uint32_t offset = 0;
  uint8_t cmdId = unpackUint8(buffer, &offset);
  cmd_response_error_code_t encoded = (cmd_response_error_code_t)unpackUint8(buffer, &offset);
  float data1 = unpackFloat(buffer, &offset);
  uint32_t data2 = unpackUint32(buffer, &offset);

  EXPECT_EQ(cmdId, (uint8_t)CMD_EXEC_OBC_RESET);
  EXPECT_EQ(encoded, CMD_RESPONSE_SUCCESS);
  EXPECT_EQ(data2, unpackedResponse.obcResetResponse.data2);
  EXPECT_EQ(data1, unpackedResponse.obcResetResponse.data1);
}

TEST(pack_unpack_command_responses, unpackCommandResponse) {
  cmd_unpacked_response_t unpackedResponse = {
      .errCode = CMD_RESPONSE_ERROR, .cmdId = CMD_EXEC_OBC_RESET, .obcResetResponse = {.data1 = 0.02, .data2 = 2}};

  uint8_t buffer[CMD_RESPONSE_MAX_PACKED_SIZE] = {0};
  obc_gs_error_code_t errCode = packCommandResponse(&unpackedResponse, buffer);

  ASSERT_EQ(errCode, OBC_GS_ERR_CODE_SUCCESS);

  cmd_unpacked_response_t deserializedResponse = {CMD_RESPONSE_SUCCESS};

  errCode = unpackCommandResponse(buffer, &deserializedResponse);
  ASSERT_EQ(errCode, OBC_GS_ERR_CODE_SUCCESS);

  EXPECT_EQ(deserializedResponse.cmdId, unpackedResponse.cmdId);
  EXPECT_EQ(deserializedResponse.errCode, unpackedResponse.errCode);
  EXPECT_EQ(deserializedResponse.obcResetResponse.data1, unpackedResponse.obcResetResponse.data1);
  EXPECT_EQ(deserializedResponse.obcResetResponse.data2, unpackedResponse.obcResetResponse.data2);
}

TEST(pack_unpack_command_responses, packInvalidCommand) {
  cmd_unpacked_response_t unpackedResponse = {
      .errCode = CMD_RESPONSE_ERROR, .cmdId = CMD_DOWNLINK_TELEM, .obcResetResponse = {0}};

  uint8_t buffer[CMD_RESPONSE_MAX_PACKED_SIZE] = {0};
  obc_gs_error_code_t errCode = packCommandResponse(&unpackedResponse, buffer);

  ASSERT_EQ(errCode, OBC_GS_ERR_CODE_SUCCESS);
  EXPECT_EQ(buffer[0], (uint8_t)unpackedResponse.cmdId);
  EXPECT_EQ(buffer[1], CMD_RESPONSE_ERROR);

  cmd_unpacked_response_t deserializedResponse = {CMD_RESPONSE_SUCCESS};
  errCode = unpackCommandResponse(buffer, &deserializedResponse);
  ASSERT_EQ(errCode, OBC_GS_ERR_CODE_SUCCESS);

  EXPECT_EQ(deserializedResponse.cmdId, unpackedResponse.cmdId);
}

TEST(pack_unpack_command_responses, unpackInvalidCommand) {
  const uint8_t maxNumber = (uint8_t)NUM_CMD_CALLBACKS;
  uint8_t buffer[CMD_RESPONSE_MAX_PACKED_SIZE] = {[0] = maxNumber};

  cmd_unpacked_response_t deserializedResponse = {CMD_RESPONSE_SUCCESS};
  obc_gs_error_code_t errCode = unpackCommandResponse(buffer, &deserializedResponse);
  EXPECT_EQ(errCode, OBC_GS_ERR_CODE_UNSUPPORTED_CMD);
}
