#pragma once

#include <stdint.h>

// Specify whether we're using R&D or production commands
#ifndef OBC_ACTIVE_POLICY
#define OBC_ACTIVE_POLICY CMD_POLICY_RND
#endif

// Policy is defined by a mask of the following flags
typedef enum {
  CMD_POLICY_RND = 0x1,   // R&D policy
  CMD_POLICY_PROD = 0x2,  // Production policy
} cmd_policy_t;

// Command options are defined by a mask of the following flags
typedef enum {
  CMD_TYPE_NORMAL = 0x1,    // Normal command
  CMD_TYPE_CRITICAL = 0x2,  // Critical command
  CMD_TYPE_ARO = 0x4,       // ARO command
} cmd_opt_t;

/*
 * Command IDs
 *
 * - The command IDs are used to identify the command in the command message.
 * - All command IDs must be unique and fit in a uint8_t.
 */

/* Used to indicate that the command is invalid.
   It should not have an unpack function. */
// NOTE: Update the python versions as well when commands are added
typedef enum {
  CMD_END_OF_FRAME = 0x00,
  CMD_EXEC_OBC_RESET,
  CMD_RTC_SYNC,
  CMD_DOWNLINK_LOGS_NEXT_PASS,
  CMD_MICRO_SD_FORMAT,
  CMD_PING,
  CMD_DOWNLINK_TELEM,
  CMD_UPLINK_DISC,
  NUM_CMD_CALLBACKS,
} cmd_callback_id_t;
