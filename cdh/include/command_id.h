#ifndef CDH_INCLUDE_COMMAND_ID_H_
#define CDH_INCLUDE_COMMAND_ID_H_

#include "obc_assert.h"
#include <stdint.h>

// Specify whether we're using R&D or production commands
#ifndef OBC_ACTIVE_POLICY
#define OBC_ACTIVE_POLICY CMD_POLICY_RND
#endif

typedef enum {
    CMD_POLICY_RND      = 0, // R&D policy
    CMD_POLICY_PROD     = 1, // Production policy
} cmd_policy_t;

typedef enum {
    CMD_TYPE_NORMAL     = 0x1, // Normal command
    CMD_TYPE_CRITICAL   = 0x2, // Critical command
    CMD_TYPE_ARO        = 0x4, // ARO command
} cmd_opt_t;

/* 
 * Command IDs
 * 
 * - The command IDs are used to identify the command in the command message.
 * - All command IDs must be unique.
*/

/* Used to indicate that the command is invalid.
   It should not have an unpack function. */
#define CMD_NONE                        (uint8_t) 0

#define CMD_EXEC_OBC_RESET              (uint8_t) 1
#define CMD_RTC_SYNC                    (uint8_t) 2
#define CMD_DOWNLINK_LOGS_NEXT_PASS     (uint8_t) 3

#endif // CDH_INCLUDE_COMMAND_ID_H_
