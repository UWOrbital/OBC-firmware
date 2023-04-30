#ifndef CDH_INCLUDE_COMMAND_ID_H_
#define CDH_INCLUDE_COMMAND_ID_H_

#include "obc_assert.h"
#include <stdint.h>

/*--------------------------*/
/*   Command ID Definitions */
/*--------------------------*/

/* Used to indicate that the command is invalid.
   It should not have an unpack function. */
#define CMD_NONE                        (uint8_t) 0

#define CMD_EXEC_OBC_RESET              (uint8_t) 1
#define CMD_RTC_SYNC                    (uint8_t) 2
#define CMD_DOWNLINK_LOGS_NEXT_PASS     (uint8_t) 3
#define CMD_END_OF_TRANSMISSION         (uint8_t) 255


#endif // CDH_INCLUDE_COMMAND_ID_H_
