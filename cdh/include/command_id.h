#ifndef CDH_INCLUDE_COMMAND_ID_H_
#define CDH_INCLUDE_COMMAND_ID_H_

#include "obc_assert.h"
#include <stdint.h>

// Update this number when adding a new command
#define NUM_CMDS 3

// Command IDs
#define CMD_EXEC_OBC_RESET              (uint8_t) 0
#define CMD_RTC_SYNC                    (uint8_t) 1
#define CMD_DOWNLINK_LOGS_NEXT_PASS     (uint8_t) 2


STATIC_ASSERT(NUM_CMDS < 256, "NUM_CMDS must be less than 256");

#endif // CDH_INCLUDE_COMMAND_ID_H_