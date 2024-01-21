
#include "obc_gs_commands_response_pack.h"
#include "obc_gs_commands_response_unpack.h"
#include "obc_gs_commands_response.h"

#include "obc_gs_errors.h"

#include <gtest/gtest.h>
#include <stdbool.h>

// CMD_EXEC_OBC_RESET
TEST(packCommandResponse, unpackCommandResponse) {
  cmd_unpacked_response_t unpackedResponse = {
      .cmdId = execObCResetCmd, .success = true, .obcResetResponse = {.data1 = 0.02, .data2 = 2}};

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
