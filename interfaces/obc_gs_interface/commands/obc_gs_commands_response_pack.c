
#include "data_pack_utils.h"
#include "obc_gs_commands_response_pack.h"
#include "obc_logging.h"

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

typedef obc_gs_error_code_t (*pack_cmd_handler_t)(cmd_unpacked_response_t*, uint8_t*, uint32_t*);

/**
 * @brief Sample prototype for functions callback functions used to pack data.
 * @param response The unpacked response which is serialized.
 * @param buffer A buffer of bytes to which to write the packed data
 * @param offset A offset of the byte at which we should begin to pack the data.
 * @return Error code indicating if the packing operation was successful. Error code OBC_GS_ERR_CODE_INVALID_ARG if the
 * buffer is NULL.
 */
static obc_gs_error_code_t packObcResetResponse(cmd_unpacked_response_t* response, uint8_t* buffer, uint32_t* offset);

static const pack_cmd_handler_t packHandlers[NUM_CMD_CALLBACKS] = {[CMD_EXEC_OBC_RESET] = packObcResetResponse};

obc_gs_error_code_t packCommandResponse(cmd_unpacked_response_t* response, uint8_t* buffer) {
  if (buffer == NULL || response == NULL) return OBC_GS_ERR_CODE_INVALID_ARG;

  uint32_t offset = 0;
  packUint8(((uint8_t)response->cmdId) & 0xFF, buffer, &offset);
  packUint8((uint8_t)(response->errCode & 0xFF), buffer, &offset);
  pack_cmd_handler_t handler = packHandlers[response->cmdId];
  if (handler == NULL) return OBC_GS_ERR_CODE_SUCCESS;

  obc_gs_error_code_t errCode = ((handler)(response, buffer, &offset));
  return errCode;
}

static obc_gs_error_code_t packObcResetResponse(cmd_unpacked_response_t* response, uint8_t* buffer, uint32_t* offset) {
  if (buffer == NULL || offset == NULL || response == NULL) return OBC_GS_ERR_CODE_INVALID_ARG;
  packFloat(response->obcResetResponse.data1, buffer, offset);
  packUint32(response->obcResetResponse.data2, buffer, offset);
  return OBC_GS_ERR_CODE_SUCCESS;
}
