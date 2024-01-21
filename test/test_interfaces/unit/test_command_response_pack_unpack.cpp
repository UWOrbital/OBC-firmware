
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

  EXPECT_EQ(encoded, 0xF1);
  EXPECT_EQ(data1, 0.02);
  EXPECT_EQ(data2, 2);
}

// CMD_EXEC_OBC_RESET
/*
TEST(pack_unpack_command_responses, unpackCommandResponse) {
  std::cout << " GOT HERE " << std::endl;
  cmd_unpacked_response_t unpackedResponse = {
      .success = true, .cmdId = execObCResetCmd, .obcResetResponse = {.data1 = 0.02, .data2 = 2}};

  uint8_t buffer[CMD_RESPONSE_MAX_PACKED_SIZE] = {0};
  obc_gs_error_code_t errCode = packCommandResponse(unpackedResponse, buffer);
  std::cout << buffer[0] << std::endl;
  ASSERT_EQ(errCode, OBC_GS_ERR_CODE_SUCCESS);

  cmd_unpacked_response_t deserializedResponse = {0};
  errCode = unpackCommandResponse(buffer, &deserializedResponse);
  ASSERT_EQ(errCode, OBC_GS_ERR_CODE_SUCCESS);

  EXPECT_EQ(deserializedResponse.cmdId, unpackedResponse.cmdId);
  EXPECT_EQ(deserializedResponse.success, unpackedResponse.success);
  EXPECT_EQ(deserializedResponse.obcResetResponse.data1, unpackedResponse.obcResetResponse.data1);
  EXPECT_EQ(deserializedResponse.obcResetResponse.data2, unpackedResponse.obcResetResponse.data2);
}
*/
