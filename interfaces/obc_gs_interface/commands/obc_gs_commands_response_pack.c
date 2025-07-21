
#include "data_pack_utils.h"
#include "obc_gs_commands_response.h"
#include "obc_gs_errors.h"

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define MAX_CMD_RESPONSE_DATA_SIZE 220

obc_gs_error_code_t packCmdResponse(cmd_response_t* response, uint8_t* buffer) {
  if (response == NULL || buffer == NULL) return OBC_GS_ERR_CODE_INVALID_ARG;

  // Here we check if the length of the data is greater than what can be sent
  if (response->dataLen > MAX_CMD_RESPONSE_DATA_SIZE) return OBC_GS_ERR_CODE_INVALID_ARG;

  uint32_t offset = 0;
  packUint8((uint8_t)response->cmdId, buffer, &offset);
  packUint8((uint8_t)response->errCode, buffer, &offset);
  packUint8((uint8_t)response->dataLen, buffer, &offset);
  memcpy(&buffer[offset], response->data, response->dataLen);

  return OBC_GS_ERR_CODE_SUCCESS;
}
