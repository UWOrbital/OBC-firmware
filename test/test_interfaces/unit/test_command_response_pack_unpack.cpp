
#include "obc_gs_commands_response_pack.h"
#include "obc_gs_commands_response_unpack.h"
#include "obc_gs_commands_response.h"

#include "data_unpack_utils.h"

#include "obc_gs_errors.h"
#include <iostream>

#include <gtest/gtest.h>
#include <stdbool.h>

TEST(pack_unpack_command_responses, packResponse) {
  cmd_unpacked_response_t unpackedResponse = {
      .success = true, .cmdId = execObCResetCmd, .obcResetResponse = {.data1 = 0.02, .data2 = 2}};

  uint8_t buffer[CMD_RESPONSE_MAX_PACKED_SIZE] = {0};
  obc_gs_error_code_t errCode = packCommandResponse(unpackedResponse, buffer);
  ASSERT_EQ(errCode, OBC_GS_ERR_CODE_SUCCESS);

  uint32_t offset = 0;
  uint8_t encoded = unpackUint8(buffer, &offset);
  float data1 = unpackFloat(buffer, &offset);
  uint32_t data2 = unpackUint32(buffer, &offset);

  EXPECT_EQ(encoded, 0x01);
  EXPECT_EQ(data2, 2);
  EXPECT_EQ(data1, (float)0.02);
}

TEST(pack_unpack_command_responses, unpackCommandResponse) {
  cmd_unpacked_response_t unpackedResponse = {
      .success = true, .cmdId = execObCResetCmd, .obcResetResponse = {.data1 = 0.02, .data2 = 2}};

  uint8_t buffer[CMD_RESPONSE_MAX_PACKED_SIZE] = {0};
  obc_gs_error_code_t errCode = packCommandResponse(unpackedResponse, buffer);

  ASSERT_EQ(errCode, OBC_GS_ERR_CODE_SUCCESS);

  cmd_unpacked_response_t deserializedResponse = {0};

  errCode = unpackCommandResponse(buffer, &deserializedResponse);
  ASSERT_EQ(errCode, OBC_GS_ERR_CODE_SUCCESS);

  EXPECT_EQ(deserializedResponse.cmdId, unpackedResponse.cmdId);
  EXPECT_EQ(deserializedResponse.success, unpackedResponse.success);
  EXPECT_EQ(deserializedResponse.obcResetResponse.data1, unpackedResponse.obcResetResponse.data1);
  EXPECT_EQ(deserializedResponse.obcResetResponse.data2, unpackedResponse.obcResetResponse.data2);
}

TEST(pack_unpack_command_responses, packInvalidCommand) {
  cmd_unpacked_response_t unpackedResponse = {.success = false, .cmdId = donwlinkTelemCmd, .obcResetResponse = {0}};

  uint8_t buffer[CMD_RESPONSE_MAX_PACKED_SIZE] = {0};
  obc_gs_error_code_t errCode = packCommandResponse(unpackedResponse, buffer);

  ASSERT_EQ(errCode, OBC_GS_ERR_CODE_SUCCESS);
  EXPECT_EQ(buffer[0], 0x50);

  cmd_unpacked_response_t deserializedResponse = {0};
  errCode = unpackCommandResponse(buffer, &deserializedResponse);
  ASSERT_EQ(errCode, OBC_GS_ERR_CODE_SUCCESS);

  EXPECT_EQ(deserializedResponse.cmdId, unpackedResponse.cmdId);
}

TEST(pack_unpack_command_responses, unpackInvalidCommand) {
  const uint8_t maxNumber = ((uint8_t)NUM_CMD_CALLBACKS << CMD_ID_SHIFT) | 0x01;
  uint8_t buffer[CMD_RESPONSE_MAX_PACKED_SIZE] = {[0] = maxNumber};

  cmd_unpacked_response_t deserializedResponse = {0};
  obc_gs_error_code_t errCode = unpackCommandResponse(buffer, &deserializedResponse);
  EXPECT_EQ(errCode, OBC_GS_ERR_CODE_UNSUPPORTED_CMD);
}
