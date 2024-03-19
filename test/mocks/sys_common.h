/**
 * @brief Reasons for resetting the system
 */
typedef enum {
  RESET_REASON_UNKNOWN,
  RESET_REASON_POWER_ON,
  RESET_REASON_CMD_EXEC_OBC_RESET,
  RESET_REASON_FS_FAILURE,
  RESET_REASON_STACK_CHECK_FAIL,
  RESET_REASON_ICEPICK_RESET,
  RESET_REASON_CPU_RESET,
  RESET_REASON_SW_RESET,
  RESET_REASON_OSC_FAILURE_RESET,
  RESET_REASON_EXTERNAL_RESET,
  RESET_REASON_DIG_WATCHDOG_RESET = 25  // should be kept last so that 25-50 can be used to specify the task that failed
  /* Task IDs are mapped as RESET_REASON_ERROR_CODE_OFFSET + RESET_REASON_DIG_WATCHDOG_RESET + enum value of task in
     obc_scheduler_config_id_t + 1 */
} obc_reset_reason_t;
