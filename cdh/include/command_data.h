#ifndef CDH_INCLUDE_COMMAND_DATA_H_
#define CDH_INCLUDE_COMMAND_DATA_H_

#include "obc_logging.h"

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
    log_level_t logLevel;
} downlink_logs_next_pass_cmd_data_t;

// CMD_PAYLOAD_CAPTURE
typedef struct {
    uint8_t regDat1;
    uint8_t regDat2;
} payload_capture_cmd_data_t;

/* -------------------------- */
/*   Command Message Struct   */
/* -------------------------- */

typedef struct {
    union {
        rtc_sync_cmd_data_t rtcSync;
        downlink_logs_next_pass_cmd_data_t downlinkLogsNextPass;
        payload_capture_cmd_data_t payloadCapture;
    };
    
    uint32_t timestamp; // Unix timestamp in seconds
    bool isTimeTagged; // If true, command will be executed at timestamp

    uint8_t id; // Command ID
} cmd_msg_t;

#endif // CDH_INCLUDE_COMMAND_DATA_H_
