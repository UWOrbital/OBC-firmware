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

1. Create a static decleration for the command's pack function on the line right before the type def that defines `pack_func_t` (`typedef void (*pack_func_t)(uint8_t*, uint32_t*, const cmd_msg_t*);`). The delecration should be done in the format used in the code snippet below. Make sure to replace `CmdName` with the name of the command being packed. Additionally, add a comment a line before the delecration that spells out the command enum the function is for.
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
    2. If your command does have data, then we have to call specific data pack functions to pack said data. Make sure to call the right data pack function for the right datatype. For example, `CMD_RTC_SYNC` has additional data, `rtcSync.unixTime` which is a `uint32_t`. As such, we would call the `packUint32()` function to pack `rtcSync.unixTime` for us into bytes.
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

### Step 4: Adding the your command's unpack function
In addition to a pack function, each command has it's own unpack function. Contrary to pack, the unpack function decodes an array of bytes into the `cmd_msg_t` struct. This is done by just reversing the bitshifts applied. To learn more you can look at `obc_gs_command_unpack.c` and `data_unpack_utils.c`.

1. Create a static decleration for the command's unpack function on the line right before the type def that defines `unpack_func_t` (`typedef void (*unpack_func_t)(const uint8_t*, uint32_t*, cmd_msg_t*);`). The delecration should be done in the format used in the code snippet below. Make sure to replace `CmdName` with the name of the command being packed. Additionally, add a comment a line before the delecration that spells out the command enum the function is for.
```c
// CMD_NAME
static void unpackCmdNameCmdData(const uint8_t* buffer, uint32_t* offset, cmd_msg_t* msg);

// Example: Unpack function for CMD_RTC_SYNC
// CMD_RTC_SYNC
static void unpackRtcSyncCmdData(const uint8_t* buffer, uint32_t* offset, cmd_msg_t* msg);
```
2. Now let's define the actual function body at the end of the file. Again make sure to add a comment the line before with the command enum that the function is for.
```c
// CMD_NAME
static void unpackCmdNameCmdData(const uint8_t* buffer, uint32_t* offset, cmd_msg_t* cmdMsg) {

}

// Example: Unpack function body for CMD_RTC_SYNC
// CMD_RTC_SYNC
static void unpackRtcSyncCmdData(const uint8_t* buffer, uint32_t* offset, cmd_msg_t* cmdMsg) {

}
```
3. The manner in which you do this step depends on if the command your unpacking has additional data or not.
    1. If your command has no data, simply add a comment saying `// No data to pack`.
    ```c
    // CMD_NAME
    static void unpackCmdNameCmdData(const uint8_t* buffer, uint32_t* offset, cmd_msg_t* cmdMsg) {
      // No data to unpack
    }
    ```
    2. If your command has data, then we need to call data unpack functions in a specific order to unpack the necessary data. Like we did in the pack function, make sure to call the right data unpack function for the right datatype. For example, `CMD_RTC_SYNC` has additional data, `rtcSync.unixTime` which is a `uint32_t`. As such, we would call the `unpackUint32()` function to pack `rtcSync.unixTime` for us into bytes. In this case, we will be assigning the return of the data unpack functions to their respective variable in the `cmdMsg` struct (of type `cmd_msg_t`) which is passed into the unpack function as a parameter. **Be sure to call the respective data unpack functions in the order you called the data pack functions in the pack function of the commands.**
    ```c
    // CMD_RTC_SYNC
    static void unpackRtcSyncCmdData(const uint8_t* buffer, uint32_t* offset, cmd_msg_t* cmdMsg) {
      cmdMsg->rtcSync.unixTime = unpackUint32(buffer, offset);
      // You can unpack any extra data by calling additional data unpack functions and assigning their returns to the variables necessary. The arguments passed in remain the same.
    }
    ```
> [!WARNING]
> Call the data unpack functions in the same order you called the data pack functions in the command's pack function. This will make sure that the data packed in unpacked correctly.
>
> For example, say we have `CMD_DOWNLOAD_DATA` which has the following pack function...
> ```c
> // CMD_DOWNLOAD_DATA
> static void packDownloadDataCmdData(uint8_t* buffer, uint32_t* offset, const cmd_msg_t* cmdMsg) {
>   packUint8((uint8_t)cmdMsg->downloadData.programmingSession, buffer, offset);
>   packUint16(cmdMsg->downloadData.length, buffer, offset);
>   packUint32(cmdMsg->downloadData.address, buffer, offset);
> }
> ```
> The unpack function would be...
> ```c
> // CMD_DOWNLOAD_DATA
> static void unpackDownloadDataCmdData(const uint8_t* buffer, uint32_t* offset, cmd_msg_t* cmdMsg) {
>   cmdMsg->downloadData.programmingSession = unpackUint8(buffer, offset);
>   cmdMsg->downloadData.length = unpackUint16(buffer, offset);
>   cmdMsg->downloadData.address = unpackUint32(buffer, offset);
> }
> ```

> [!NOTE]
> As of writing this procedure the code base has the following unpack functions implemented and at your disposal:
> `unpackUint8()`, `unpackUint16()`, `unpackUint32()`, `unpackInt8()`, `unpackInt16()`, `unpackInt32()` and `unpackFloat()`.

### Step 5: Adding a command callback
To make the command actually do something, the code base uses callbacks. Callbacks, in this case, are simply the functions, with the command's functionality, that are called when the commands are processed. Since the required commands and the functionality of those commands varies from bootloader to app, we have two files that define callbacks: `command_callbacks.c` (for the app) and `bl_command_callbacks.c` (for the bootloader).

1. To define a callback first determine if the command will run on the booloader or the app or both. If the command only runs on bootloader, do the following steps in `bl_command_callbacks.c` only; if the command only runs on the app, do the following steps in `command_callbacks.c`; and if the app runs on both the bootloader and app do the following steps in both files.

2. Before the definition of the function pointer array, `cmdsConfig[]` define the callback function in the format outlined in the code snippet. Be sure to replace `cmdName` with the actual name of the command.

```c
static obc_error_code_t cmdNameCmdCallback(cmd_msg_t *cmd, uint8_t *responseData, uint8_t *responseDataLen) {
  if (cmd == NULL || responseData == NULL || responseDataLen == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  return OBC_ERR_CODE_SUCCESS;
}
```

3. Then in `cmdsConfig` we add the command and it's respective callback, while specifying some options as seen in the following snippet. Replace `CMD_NAME` with the command enum and `cmdNameCmdCallback` with the correct callback function
    1. You will notice that we specify `CMD_POLICY_PROD`. `CMD_POLICY_PROD` is a `cmd_policy_t` enum that just specifies if the command is for research and development (`CMD_POLICY_RND`) or if the command is meant to be used in production (`CMD_POLICY_PROD`). While this is currently not implemented, it is good to follow the convention so everything works as expected when this does get implemented.
    2. We also specify `CMD_TYPE_NORMAL` which is a `cmd_opt_t` enum that defines the type the command. As of writing this procedure there are three command types: `CMD_TYPE_NORMAL`, `CMD_TYPE_CRITICAL` and `CMD_TYPE_ARO`.
```c
const cmd_info_t cmdsConfig[] = {
    [CMD_END_OF_FRAME] = {NULL, CMD_POLICY_PROD, CMD_TYPE_NORMAL},
    [CMD_EXEC_OBC_RESET] = {execObcResetCmdCallback, CMD_POLICY_PROD, CMD_TYPE_CRITICAL},
    [CMD_RTC_SYNC] = {rtcSyncCmdCallback, CMD_POLICY_PROD, CMD_TYPE_NORMAL},
    [CMD_DOWNLINK_LOGS_NEXT_PASS] = {downlinkLogsNextPassCmdCallback, CMD_POLICY_PROD, CMD_TYPE_CRITICAL},
    [CMD_MICRO_SD_FORMAT] = {microSDFormatCmdCallback, CMD_POLICY_PROD, CMD_TYPE_CRITICAL},
    [CMD_PING] = {pingCmdCallback, CMD_POLICY_PROD, CMD_TYPE_NORMAL},
    [CMD_DOWNLINK_TELEM] = {downlinkTelemCmdCallback, CMD_POLICY_PROD, CMD_TYPE_NORMAL},
    [CMD_I2C_PROBE] = {I2CProbeCmdCallback, CMD_POLICY_PROD, CMD_TYPE_NORMAL},
    [CMD_NAME] = {cmdNameCmdCallback, CMD_POLICY_PROD, CMD_TYPE_NORMAL},
};
```
4. Finally, you can add the functionality of your command in the callback function body.
```c
static obc_error_code_t cmdNameCmdCallback(cmd_msg_t *cmd, uint8_t *responseData, uint8_t *responseDataLen) {
  if (cmd == NULL || responseData == NULL || responseDataLen == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  // Implement functionality here

  return OBC_ERR_CODE_SUCCESS;
}
```

### Step 6: Adding a command response
Each command sends back its own command response. In this response you have an option to add data. If you do not have any data to send back as a response to the command, you can skip this step.

1. Specify the length of the data that you want to send back by setting the `responseDataLen` variable.

> [!CAUTION]
> The maximum length of a response can only be 220. Do not go beyond this length (otherwise, you will cause errors)!

```c
static obc_error_code_t cmdNameCmdCallback(cmd_msg_t *cmd, uint8_t *responseData, uint8_t *responseDataLen) {
  if (cmd == NULL || responseData == NULL || responseDataLen == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  // You can change 200 to be any number from 1 to 220
  *resonseDataLen = 200;
  // Implement functionality here

  return OBC_ERR_CODE_SUCCESS;
}
```

2. Then you can populate data by using the `responseData` pointer as an array pointer.
```c
static obc_error_code_t cmdNameCmdCallback(cmd_msg_t *cmd, uint8_t *responseData, uint8_t *responseDataLen) {
  if (cmd == NULL || responseData == NULL || responseDataLen == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  // Example of setting one byte
  *resonseDataLen = 1;
  responseData[0] = 0xFF;

  // Implement functionality here

  return OBC_ERR_CODE_SUCCESS;
}
```