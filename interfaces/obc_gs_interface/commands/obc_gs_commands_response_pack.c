
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "data_pack_utils.h"
#include "obc_gs_commands_response_pack.h"

typedef obc_gs_error_code_t (*pack_cmd_handler_t)(cmd_unpacked_response_t, uint8_t*, uint32_t*);
static cmd_callback_encoded_t _encodeResponse(cmd_callback_id_t id, bool success);

static obc_gs_error_code_t packObcResetResponse(cmd_unpacked_response_t response, uint8_t* buffer, uint32_t* offset);

static const pack_cmd_handler_t packHandlers[NUM_CMD_CALLBACKS] = {[execObCResetCmd] = packObcResetResponse};

obc_gs_error_code_t packCommandResponse(cmd_unpacked_response_t response, uint8_t* buffer) {
  cmd_callback_encoded_t encoded = _encodeResponse(response.cmdId, response.success);
  uint32_t offset = 0;

  packUint8((uint8_t)encoded, buffer, &offset);
  pack_cmd_handler_t handler = packHandlers[response.cmdId];

  obc_gs_error_code_t errCode = ((handler)(response, buffer, &offset));
  return errCode;
}

static cmd_callback_encoded_t _encodeResponse(cmd_callback_id_t id, bool success) {
  uint8_t successBit = (success) ? 0x01 : 0x00;
  cmd_callback_encoded_t response = 0;
  response |= ((uint8_t)id << CMD_ID_SHIFT);
  response |= successBit;
  return response;
}

static obc_gs_error_code_t packObcResetResponse(cmd_unpacked_response_t response, uint8_t* buffer, uint32_t* offset) {
  if (buffer == NULL || offset == NULL) return OBC_GS_ERR_CODE_INVALID_ARG;
  packFloat(response.obcResetResponse.data1, buffer, offset);
  packUint32(response.obcResetResponse.data2, buffer, offset);
  return OBC_GS_ERR_CODE_SUCCESS;
}
