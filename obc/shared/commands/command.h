#pragma once

#include "obc_errors.h"
#include "obc_gs_command_data.h"

typedef obc_error_code_t (*cmd_callback_t)(cmd_msg_t *);

typedef struct {
  cmd_callback_t callback;
  cmd_policy_t policy;
  cmd_opt_t opts;  // Mask of command options
} cmd_info_t;

/**
 * @brief Verifies a command and assigns it's information to the currCmdInfo parameter.
 *
 * @param cmd Pointer to the command message
 * @param currCmdInfo Pointer to the command information
 * @param cmdProgressTracker a static array of booleans that keeps track of each command's progress (refer to
 * command_manager.c)
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if successful, otherwise an error code
 */
obc_error_code_t verifyCommand(cmd_msg_t *cmd, cmd_info_t *currCmdInfo);

/**
 * @brief Processes non time tagged commands
 *
 * @param cmd Pointer to the command message
 * @param currCmdInfo Pointer to the command information
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if successful, otherwise an error code
 */
obc_error_code_t processNonTimeTaggedCommand(cmd_msg_t *cmd, cmd_info_t *currCmdInfo);

/**
 * @brief Processes time tagged commands
 *
 * @param cmd Pointer to the command message
 * @param currCmdInfo Pointer to the command information
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if successful, otherwise an error code
 */
obc_error_code_t processTimeTaggedCommand(cmd_msg_t *cmd, cmd_info_t *currCmdInfo);
