
#pragma once

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "obc_gs_errors.h"
#include "obc_gs_commands_response.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Pack a buffer of chars from a struct of type cmd_unpacked_response_t representing the response of a command.
 * @param buffer A buffer of bytes to which to write the packed data
 * @return Error code indicating if the packing operation was successful. Error code OBC_GS_ERR_CODE_INVALID_ARG if the
 * buffer is NULL.
 */
obc_gs_error_code_t packCommandResponse(cmd_unpacked_response_t response, uint8_t *buffer);

#ifdef __cplusplus
}
#endif
