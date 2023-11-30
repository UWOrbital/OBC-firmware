#pragma once

/**
 * @brief Reasons for resetting the system
 */
typedef enum {
  RESET_REASON_TESTING,             // For testing purposes
  RESET_REASON_CMD_EXEC_OBC_RESET,  // Reset due to command execution
  RESET_REASON_FS_FAILURE,          // File system operation failed
  RESET_REASON_STACK_CHECK_FAIL,    // Stack canary check failed
} obc_reset_reason_t;

/**
 * @brief reset the systems
 *
 * @param obc_reset_reason_t - the reason to reset system
 */
void resetSystem(obc_reset_reason_t reason);
