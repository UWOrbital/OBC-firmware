#include "data_unpack_utils.h"
#include "obc_gs_commands_response_unpack.h"
#include "obc_gs_command_id.h"

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

typedef obc_gs_error_code_t (*unpack_cmd_handler_t)(cmd_unpacked_response_t*, uint8_t*, uint32_t*);

/**
 * @brief Unpack a buffer of chars to a struct of type cmd_unpacked_response_t representing the response of a command.
 * @param response The unpacked response that needs to be extracted from the packed data.
 * @param buffer A buffer of bytes from which to extract the data.
 * @param offset A offset of the byte at which we should begin to unpack the data.
 * @return Error code indicating if the write was successful. OBC_GS_ERR_CODE_INVALID_ARG if any of the pointers are
 *NULL
 **/
static obc_gs_error_code_t unpackObcResetResponse(cmd_unpacked_response_t* response, uint8_t* buffer, uint32_t* offset);

static const unpack_cmd_handler_t unpackHandlers[NUM_CMD_CALLBACKS] = {[CMD_EXEC_OBC_RESET] = unpackObcResetResponse};

obc_gs_error_code_t unpackCommandResponse(uint8_t* buffer, cmd_unpacked_response_t* response) {
  if (response == NULL || buffer == NULL) return OBC_GS_ERR_CODE_INVALID_ARG;

  uint32_t offset = 0;
  response->cmdId = unpackUint8(buffer, &offset);
  if (response->cmdId >= NUM_CMD_CALLBACKS) return OBC_GS_ERR_CODE_UNSUPPORTED_CMD;

  obc_gs_error_code_t errCode = 0;
  response->errCode = (cmd_response_error_code_t)unpackUint8(buffer, &offset);
  unpack_cmd_handler_t handler = unpackHandlers[response->cmdId];
  if (handler == NULL) return OBC_GS_ERR_CODE_SUCCESS;

  errCode = ((handler)(response, buffer, &offset));
  return errCode;
}

static obc_gs_error_code_t unpackObcResetResponse(cmd_unpacked_response_t* response, uint8_t* buffer,
                                                  uint32_t* offset) {
  if (response == NULL || buffer == NULL || offset == NULL) return OBC_GS_ERR_CODE_INVALID_ARG;
  response->obcResetResponse.data1 = unpackFloat(buffer, offset);
  response->obcResetResponse.data2 = unpackUint32(buffer, offset);
  return OBC_GS_ERR_CODE_SUCCESS;
}
