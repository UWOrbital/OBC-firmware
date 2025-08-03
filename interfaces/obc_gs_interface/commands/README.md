# Command Responses Guide
The following is a guide to adding commands to ensure that they work in the python and c sections of this code base.

## Table of Contents

## Adding commands to the C Section
### Step 1: Adding the command id
1. Navigate to `obc_gs_command_id.h` and in the `cmd_callback_id_t` enum struct, add your command id. This must be **before** `NUM_CMD_CALLBACKS` (since we use it to count the number of commands throughout the codebase). The command should be written in uppercase with underscores as spaces and should be prefixed with `CMD`. As an example, `CMD_MAPLE_LEAFS` is correct but `CmdMapleLeafs`, `CMDMAPLELEAFS`, `cmd_maple_leafs` are not.
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
>[!NOTE]
> By default each command has two data fields: `timestamp` and `isTimeTagged` which specify whether the command needs to be executed at a certain time or not.
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
> In `obc_gs_command_data.h` there are two sections for command data structures: one for the bootloader and another for the actual app. Depending on what your command is for, place your data struct in the right section of the file (this helps keep the code clean).

### Step 3: Adding your command's pack function
Each command has its respective pack function to turn the command into transmittable bytes. The conversion to bytes is done by packing information in the `cmd_msg_t` struct in a specific order and then bitshifting the data in certain ways. You can learn more about this by looking at the code in `obc_gs_command_pack.c` and `data_pack_utils.c`.

> [!NOTE]
> In the following steps (and later on) we add comments at specific places as a part of codestyle and readability. Since there will be a large amount of commands at some point, these comments will hopefully serve as a way to find functions quickly as the codebase grows.

1. Create a static decleration for the command's pack function near right before the type def that defines `pack_func_t` (`typedef void (*pack_func_t)(uint8_t*, uint32_t*, const cmd_msg_t*);`). The delecration should be done in the format used in the code snippet below. Make sure to replace `CmdName` with the name of the command being packed. Additionally, add a comment a line before the delecration that spells out the command enum the function is for.
```c
// CMD_NAME
static void packCmdNameCmdData(uint8_t* buffer, uint32_t* offset, const cmd_msg_t* msg);

// Example: Pack function for CMD_RTC_SYNC
// CMD_RTC_SYNC
static void packRtcSyncCmdData(uint8_t* buffer, uint32_t* offset, const cmd_msg_t* msg);
```
2. Now define the actual function body at the end of the file. Again make sure to add a comment the line before with the command enum that the function is for.
```c
// CMD_NAME
static void packCmdNameCmdData(uint8_t* buffer, uint32_t* offset, const cmd_msg_t* msg) {

}

// Example: Pack function body for CMD_RTC_SYNC
// CMD_RTC_SYNC
static void packRtcSyncCmdData(uint8_t* buffer, uint32_t* offset, const cmd_msg_t* cmdMsg) {

}
```
3. The manner in which you do this step depends on if the command your packing has additional data or not.
    1. If your command has no data, simply add a comment saying `// No data to pack`.
```c
// CMD_NAME
static void packCmdNameCmdData(uint8_t* buffer, uint32_t* offset, const cmd_msg_t* msg) {
  // No data to pack
}
```
2. If your command does have data, then we have to call specific pack functions to pack said data. Make sure to call the right pack function for the right datatype. For example, `CMD_RTC_SYNC` has additional data, `rtcSync.unixTime` which is a `uint32_t`. As such, we would call the `packUint32()` function to pack `rtcSync.unixTime` for us into bytes.
```c
// CMD_RTC_SYNC
static void packRtcSyncCmdData(uint8_t* buffer, uint32_t* offset, const cmd_msg_t* cmdMsg) {
  packUint32(cmdMsg->rtcSync.unixTime, buffer, offset);
  // You can pack any extra data by calling one of the pack functions with the data you want to pack while leaving the rest of the arguments the same
}
```
>[!TIP]
> Be mindful of the order you pack data in (i.e. what order you call the pack functions on data in) as you will have to unpack them in the same order while writing the unpack functions (don't worry there'll be a warning to remind you)

> [!NOTE]
> As of writing this procedure the code base has the following pack functions implemented and at your disposal:
> `packUint8()`, `packUint16()`, `packUint32()`, `packInt8()`, `packInt16()`, `packInt32()` and `packFloat()`