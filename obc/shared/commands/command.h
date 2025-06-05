#pragma once

#include "FreeRTOS.h"
#include "obc_errors.h"
#include "obc_gs_command_data.h"
#include "command_callbacks.h"

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
obc_error_code_t verifyCommand(cmd_msg_t *cmd, cmd_info_t *currCmdInfo, bool cmdProgressTracker[]);

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

static const cmd_info_t cmdsConfig[] = {
    [CMD_END_OF_FRAME] = {NULL, CMD_POLICY_PROD, CMD_TYPE_NORMAL},
    [CMD_EXEC_OBC_RESET] = {execObcResetCmdCallback, CMD_POLICY_PROD, CMD_TYPE_CRITICAL},
    [CMD_RTC_SYNC] = {rtcSyncCmdCallback, CMD_POLICY_PROD, CMD_TYPE_NORMAL},
    [CMD_DOWNLINK_LOGS_NEXT_PASS] = {downlinkLogsNextPassCmdCallback, CMD_POLICY_PROD, CMD_TYPE_CRITICAL},
    [CMD_MICRO_SD_FORMAT] = {microSDFormatCmdCallback, CMD_POLICY_PROD, CMD_TYPE_CRITICAL},
    [CMD_PING] = {pingCmdCallback, CMD_POLICY_PROD, CMD_TYPE_NORMAL},
    [CMD_DOWNLINK_TELEM] = {downlinkTelemCmdCallback, CMD_POLICY_PROD, CMD_TYPE_NORMAL}};

#define CMDS_CONFIG_SIZE (sizeof(cmdsConfig) / sizeof(cmd_info_t))
