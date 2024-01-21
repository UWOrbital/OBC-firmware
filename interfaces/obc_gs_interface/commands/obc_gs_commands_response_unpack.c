#pragma once

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "data_unpack_utils.h"
#include "obc_gs_commands_response_unpack.h"

typedef obc_gs_error_code_t (*unpack_cmd_handler_t)(cmd_unpacked_response_t*, uint8_t*, uint32_t*);
static obc_gs_error_code_t _decodeResponse(cmd_callback_encoded_t encodedResponse, cmd_callback_id_t* id,
                                           bool* success);

static obc_gs_error_code_t unpackObcResetResponse(cmd_unpacked_response_t* response, uint8_t* buffer, uint32_t* offset);

static const unpack_cmd_handler_t unpackHandlers[] = {[execObCResetCmd] = &unpackObcResetResponse};

// Shamelessly stolen from the other file
#define MAX_CMD_RESPONSE_ID ((sizeof(unpackHandlers) / sizeof(unpack_cmd_handler_t)) - 1)

obc_gs_error_code_t unpackCommandResponse(uint8_t* buffer, cmd_unpacked_response_t* response) {
  if (response == NULL || buffer == NULL) return OBC_GS_ERR_CODE_INVALID_ARG;

  uint32_t offset;
  cmd_callback_encoded_t encodedResp = (cmd_callback_encoded_t)unpackUint8(buffer, &offset);
  _decodeResponse(encodedResp, &response->cmdId, &offset);

  if (unpackHandlers[response->cmdId] == NULL) return OBC_GS_ERR_CODE_SUCCESS;

  unpack_cmd_handler_t handler = unpackHandlers[response->cmdId];
  obc_gs_error_code_t errCode;
  RETURN_IF_ERROR_CODE((*handler)(response, buffer, offset));
  return OBC_GS_ERR_CODE_SUCCESS;
}

static obc_gs_error_code_t _decodeResponse(cmd_callback_encoded_t encodedResponse, cmd_callback_id_t* id,
                                           bool* success) {
  *id = (cmd_callback_id_t)((encodedResponse & CMD_ID_MASK) >> CMD_ID_SHIFT);

  if (*id > MAX_CMD_RESPONSE_ID) return OBC_GS_ERR_CODE_UNSUPPORTED_CMD;
  *success = (bool)(encodedResponse & CMD_RESPONSE_SUCCESS_MASK);
}

static obc_gs_error_code_t unpackObcResetResponse(cmd_unpacked_response_t* response, uint8_t* buffer,
                                                  uint32_t* offset) {
  if (response == NULL || buffer == NULL || offset == NULL) return OBC_GS_ERR_CODE_INVALID_ARG;
  response->obcResetResponse.data1 = unpackFloat(buffer, offset);
  response->obcResetResponse.data2 = unpackUint32(buffer, offset);
  return OBC_GS_ERR_CODE_SUCCESS;
}
