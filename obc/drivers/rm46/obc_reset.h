#pragma once

/**
 * @brief Reasons for resetting the system
 */
typedef enum {
  RESET_REASON_TESTING,             // For testing purposes 0
  RESET_REASON_CMD_EXEC_OBC_RESET,  // Reset due to command execution 1
  RESET_REASON_FS_FAILURE,          // File system operation failed 2
  RESET_REASON_STACK_CHECK_FAIL,    // Stack canary check failed 3
  RESET_REASON_UNKNOWN,             // Reset reason unknown (used as a default) 4
  // Reasons from sys_startup.c
  RESET_REASON_ICEPICK_RESET,     // Icepick Reset 5
  RESET_REASON_WATCHDOG_RESET,    // Watchdog Reset 6
  RESET_REASON_CPU_RESET,         // CPU Reset 7
  RESET_REASON_SW_RESET,          // SW Reset 8
  RESET_REASON_OSC_FAILURE_RESET  // OSC Failure 9
} obc_reset_reason_t;

/**
 * @brief reset the systems
 *
 * @param obc_reset_reason_t - the reason to reset system
 */
void resetSystem(obc_reset_reason_t reason);
