
#pragma once

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "obc_gs_errors.h"
#include "obc_gs_commands_response.h"

#ifdef __cplusplus
extern "C" {
#endif

obc_gs_error_code_t packCommandResponse(cmd_unpacked_response_t response, uint8_t *buffer);

#ifdef __cplusplus
}
#endif
