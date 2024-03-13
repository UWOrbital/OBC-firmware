#pragma once

#include "obc_gs_command_id.h"

#include <stdint.h>
#include <stdbool.h>
/* -------------------------- */
/*   Command Data Structures  */
/* -------------------------- */
// CMD_RTC_SYNC
typedef struct {
  uint32_t unixTime;
} rtc_sync_cmd_data_t;

// CMD_DOWNLINK_LOGS_NEXT_PASS
typedef struct {
  uint8_t logLevel;
} downlink_logs_next_pass_cmd_data_t;

/* -------------------------- */
/*   Command Message Struct   */
/* -------------------------- */

typedef struct {
  union {
    rtc_sync_cmd_data_t rtcSync;
    downlink_logs_next_pass_cmd_data_t downlinkLogsNextPass;
  };

  uint32_t timestamp;  // Unix timestamp in seconds
  bool isTimeTagged;   // If true, command will be executed at timestamp

  cmd_callback_id_t id;  // Command ID
} cmd_msg_t;

#define MAX_CMD_MSG_SIZE sizeof(cmd_msg_t)
