#pragma once

// #include "obc_errors.h"
#include "obc_errors.h"
#include "obc_gs_command_data.h"
/**
 * @brief Sends a command to the command queue
 *
 * @param cmd Pointer to the command message
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if successful, otherwise an error code
 */
obc_error_code_t sendToCommandQueue(cmd_msg_t *cmd);
