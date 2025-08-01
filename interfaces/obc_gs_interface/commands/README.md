# Command Responses Guide
The following is a guide to adding commands to ensure that they work in the python and c sections of this code base.

## Table of Contents

## Adding commands to the C Section
### Step 1: Adding the command id
1. Navigate to `obc_gs_command_id.h` and in the `cmd_callback_id_t` enum struct, add your command id. This must be **before** `NUM_CMD_CALLBACKS` (since we use it to count the number of commands throughout the codebase).
```c
typedef enum {
  CMD_END_OF_FRAME = 0x00,
  CMD_EXEC_OBC_RESET,
  CMD_RTC_SYNC,
  CMD_DOWNLINK_LOGS_NEXT_PASS,
  CMD_MICRO_SD_FORMAT,
  CMD_PING,
  CMD_DOWNLINK_TELEM,
  CMD_UPLINK_DISC,
  CMD_SET_PROGRAMMING_SESSION,
  CMD_ERASE_APP,
  CMD_DOWNLOAD_DATA,
  CMD_VERIFY_CRC,
  CMD_RESET_BL,
  // Add command here
  NUM_CMD_CALLBACKS
} cmd_callback_id_t;
```
### Step 2: Adding command data
If your command has no data associated with it you may skip this step. An example of a command that has data is `CMD_RTC_SYNC` which takes a unixtime as data and syncs the board's RTC to said unixtime.

1. If your command does have data, create a struct to house the data in `obc_gs_command_data.h`. Continuing the `CMD_RTC_SYNC` example, the following is a struct that houses the unixtime data the command requires...
```c
// CMD_RTC_SYNC
typedef struct {
  uint32_t unixTime;
} rtc_sync_cmd_data_t;
```
2. Then add the struct you just created to the union in `cmd_msg_t`.
```c
typedef struct {
  union {
    rtc_sync_cmd_data_t rtcSync;
    downlink_logs_next_pass_cmd_data_t downlinkLogsNextPass;
    download_data_cmd_data_t downloadData;
    set_programming_session_cmd_data_t setProgrammingSession;
    // Add command struct here
  };

  uint32_t timestamp;  // Unix timestamp in seconds
  bool isTimeTagged;   // If true, command will be executed at timestamp

  cmd_callback_id_t id;  // Command ID
} cmd_msg_t;
```
> [!NOTE]
> In `obc_gs_command_data.h` there are two section for command data structures: one for the bootloader and another for the actual app. Depending on what your command is for, place your data struct in the right section of the file (this helps keep the code clean).

### Step 3: Adding your command's pack function