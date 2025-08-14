# Command Responses Guide
The following is a guide to adding commands to ensure that they work in the python and c sections of this code base.

## Table of Contents

## Adding commands to the C Side
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
## Adding commands to the Python side
For our ground station to effectively use commands, we re-define and add some logic for commands in the python side of our code base. However, the core functions are actually just wrapped from their c implementations using `ctypes`.

### Step 1: Adding the command id
Just like the c-side, the python side also defines an enum for the commands.

1. Navigate to `commands/__init__.py` and find the `CmdCallbackId` struct. Insert your command just before `NUM_CMD_CALLBACKS` and change the enum integer assignments so that the integers remain consecutive (in python you have to explicitly assign the integers that C just assigns implicitly). You can replace `CMD_NAME` with your command enum name.

```python
# Original Structure
class CmdCallbackId(IntEnum):
    """
    Enums corresponding to the C implementation of cmd_callback_id_t
    """

    CMD_END_OF_FRAME = 0
    CMD_EXEC_OBC_RESET = 1
    CMD_RTC_SYNC = 2
    CMD_DOWNLINK_LOGS_NEXT_PASS = 3
    CMD_MICRO_SD_FORMAT = 4
    CMD_PING = 5
    CMD_DOWNLINK_TELEM = 6
    CMD_UPLINK_DISC = 7
    CMD_SET_PROGRAMMING_SESSION = 8
    CMD_ERASE_APP = 9
    CMD_DOWNLOAD_DATA = 10
    CMD_VERIFY_CRC = 11
    CMD_RESET_BL = 12
    NUM_CMD_CALLBACKS = 13

# Structure with added command (Notice how the numbering changes)
class CmdCallbackId(IntEnum):
    """
    Enums corresponding to the C implementation of cmd_callback_id_t
    """

    CMD_END_OF_FRAME = 0
    CMD_EXEC_OBC_RESET = 1
    CMD_RTC_SYNC = 2
    CMD_DOWNLINK_LOGS_NEXT_PASS = 3
    CMD_MICRO_SD_FORMAT = 4
    CMD_PING = 5
    CMD_DOWNLINK_TELEM = 6
    CMD_UPLINK_DISC = 7
    CMD_SET_PROGRAMMING_SESSION = 8
    CMD_ERASE_APP = 9
    CMD_DOWNLOAD_DATA = 10
    CMD_VERIFY_CRC = 11
    CMD_RESET_BL = 12
    CMD_NAME = 13
    NUM_CMD_CALLBACKS = 14
```
### Step 2: Adding command data
Just like in the C version, we need to tell python the data that a command may store. This definition helps `ctypes` wrap the c-side functions

1. Locate the union class named `_U`. Before that class you can add the data that your command will store as a child class of the `ctypes` class `Structure`. Continuing the example of `CMD_RTC_SYNC`, it's data would be defined as the following in python:
```python
class RtcSyncCmdData(Structure):
    """
    The python equivalent class for the rtc_sync_cmd_data_t structure in the C implementation
    """

    _fields_ = [("unixTime", c_uint32)]
```
> [!NOTE]
> If one of the data fields is defined as an enum on the c-side, we can just define it as a `c_uint` on the python side (or whatever size of int the enum happens to be defined as: a `uint8_t` enum would have to be declared as a `c_uint8` in python)

2. Now go to the child class of the `ctypes` `Union` class named `_U` and add the class you just created to store the command's data as a field which is a tuple of a string name and the corresponding class.

> [!WARNING]
> The name in the field should be the same as the name used in the c files. For example, for `CMD_RTC_SYNC` the `rtcSync` field is the same as the name of the `rtcSync` struct defined on the c-side.

```python
class _U(Union):
    """
    Union class needed to create the CmdMsgType Class
    """

    _fields_ = [
        # Note how we define a tuple with the name of the variable on the c-side
        # as a string and then we add it's corresponding python class.
        ("rtcSync", RtcSyncCmdData),
        ("downlinkLogsNextPass", DownlinkLogsNextPassCmdData),
        ("downloadData", DownloadDataCmdData),
        ("setProgrammingSession", SetProgrammingSessionCmdData),
    ]
```
### Step 3: Adding a command factory function
To integrate the commands in a more pythonic style, we create command factories which are functions that take in a specific commands parameters and return the generated `CmdMsg` class. Note, `CmdMsg` is the python equivalent of the `cmd_msg_t` class on the c-side.

1. Locate the command block header (the big rectangles of hashtags with text in the middle) and locate the one that says "Command Pack and Unpack Implementations". Right before that block we can start creating the command factory function as seen in the code snippet below. Be sure to replace `cmd_name` with the name of the command your creating the factory for and update the doc string accordingly (refer to the main README for docstring style).
```python
def create_cmd_name(unixtime_of_execution: int | None = None) -> CmdMsg:
    """
    Function to create a CmdMsg structure for CMD_NAME

    :param unixtime_of_execution: A time of when to execute a certain event,
                                  by default, it is set to None (i.e. a specific
                                  time is not needed)
    :return: CmdMsg structure for CMD_NAME
    """
    cmd_msg = CmdMsg(unixtime_of_execution)
    cmd_msg.id = CmdCallbackId.CMD_NAME
    return cmd_msg
```

2. If your command does not have an associated data with it, you can skip this step. If your command does have data we need to add that to your function.
    1. Define the data your command takes in the function parameters. Be sure to just use regular python types here. If your command has any integer data fields, no matter the size, declare it as an `int` when writing a function parameter to define it. Remember to update the docstring as well. Again using `CMD_RTC_SYNC` as an example...
    ```python
    # Note how the time parameter is defined as a regular int
    def create_cmd_rtc_sync(time: int, unixtime_of_execution: int | None = None) -> CmdMsg:
        """
        Function to create a CmdMsg structure for CMD_RTC_SYNC

        :param time: Unixtime as an integer
        :param unixtime_of_execution: A time of when to execute a certain event,
                                      by default, it is set to None (i.e. a specific
                                      time is not needed)
        :return: CmdMsg structure for CMD_RTC_SYNC
        """
        cmd_msg = CmdMsg(unixtime_of_execution)
        cmd_msg.id = CmdCallbackId.CMD_RTC_SYNC
        return cmd_msg
    ```
    2. This step only applies if your integer data field is less than 32 bits (i.e. `uint8_t` or `uint16_t`). To the function we need to add some validation that verifies that the number passed into the function can actually be stored as an 8-bit or 16-bit integer, otherwise ctypes will throw an error. Note, we did not need to do this for `CMD_RTC_SYNC` because, by default, a python int is 32-bit. Thus, for this example we will be using another command called `CMD_DOWNLINK_LOGS_NEXT_PASS` which takes in a `logLevel` that is defined as a `uint8_t` on the c-side. To validate the value passed into the function we add a simple conditional that throws a `ValueError` if the number is too large as seen int he code snippet below:
    ```python
    # Notice the log level parameter follows python naming and is defined as a regular python int
    def create_cmd_downlink_logs_next_pass(log_level: int, unixtime_of_execution: int | None = None) -> CmdMsg:
        """
        Function to create a CmdMsg structure for CMD_DOWNLINK_LOGS_NEXT_PASS

        :param log_level: The Log Level for the logs
        :param unixtime_of_execution: A time of when to execute a certain event,
                                      by default, it is set to None (i.e. a specific
                                      time is not needed)
        :return: CmdMsg structure for CMD_DOWNLINK_LOGS_NEXT_PASS
        """

        # The conditional checks if the number passed in is larger than what a uint8_t can actually stores
        # and throws a ValueError with a message (Don't make these messages too long but they should be
        # descriptive so that developers can debug)
        if log_level > 255:
            raise ValueError("Log level passed is too large (cannot be encoded into a c_uint8)")
        cmd_msg = CmdMsg(unixtime_of_execution)
        cmd_msg.id = CmdCallbackId.CMD_DOWNLINK_LOGS_NEXT_PASS

        # Don't worry, this is explained in the next step
        cmd_msg.downlinkLogsNextPass.logLevel = c_uint8(log_level)
        return cmd_msg
    ```
    3. Finally, we assign the parameter to the right variable in the `cmd_msg` class variable defined in the function. You'll notice that the data fields in `cmd_msg` are made of types from `ctypes` while the parameter is a regular python `int`. Thus we use functions provided by `ctypes` to convert them to the right type. Note that these functions will fail if the value being converted is invalid (i.e. too large/small). For `CMD_RTC_SYNC`, this means converting the `time` variable to a `c_uint32` using the `c_uint32()` function. Note that we access the variable by using the string names we defined in the tuples for the `ctypes` class `_fields_` and regular python dot syntax.

    > [!NOTE]
    > There are functions for each possible type in c, defined by `ctypes`. Consult the `ctypes` documentation to see how they are defined!

    ```python
    def create_cmd_rtc_sync(time: int, unixtime_of_execution: int | None = None) -> CmdMsg:
        """
        Function to create a CmdMsg structure for CMD_RTC_SYNC

        :param time: Unixtime as an integer
        :param unixtime_of_execution: A time of when to execute a certain event,
                                      by default, it is set to None (i.e. a specific
                                      time is not needed)
        :return: CmdMsg structure for CMD_RTC_SYNC
        """
        cmd_msg = CmdMsg(unixtime_of_execution)
        cmd_msg.id = CmdCallbackId.CMD_RTC_SYNC
        # Here we convert and assign the parameter.
        # If you recall, we defined the union field and gave it the name "rtcSync"
        # and then gave the variable in that class the name "unixTime", corresponding
        # to the c names.
        cmd_msg.rtcSync.unixTime = c_uint32(time)
        return cmd_msg
    ```

### Step 4: Adding command responses
If the command response being sent by the command has no data, you can skip this step. Otherwise we define a class in python to store the data nicely and in a more usable format.

1. Locate the `command_response_classes.py` file. Right after the last class we can define a new one in the format specified in the code snipped. Make sure to replace `CmdName` and `CMD_NAME` with the name of your command. Note, this is a child class of the base class used to store command responses `CmdRes` and is defined with the `dataclass` decorator. Additionally, we override the string method to help us better format the response (this is especially useful with the ground station CLI).
```python
@dataclass
class CmdCmdNameRes(CmdRes):
    """
    Class for storing the response to CMD_NAME
    """

    def __str__(self) -> str:
        """
        Overriding the str method for a better representation of what's happening
        """
        formatted_string = super().__str__()
        return formatted_string
```
2. Before the override for the string method, add in the data fields that your command response will contain. Note, we do add a type hint since this is specifically a dataclass. For example, `CMD_RTC_SYNC` will send a response that contains the unixtime of the board when the command was sent. Make sure to update the docstring as seen in the code snippet below:
```python
@dataclass
class CmdRtcSyncRes(CmdRes):
    """
    Child class for storing the response to the CMD_RTC_SYNC
    :param board_unixtime: The time on the board when the sync command was sent
    :type board_unixtime: int
    """

    board_unixtime: int

    def __str__(self) -> str:
        """
        Overriding the str method for a better representation of what's happening
        """
        formatted_string = super().__str__()

        return formatted_string
```
3. Now we fully define the string override by appending the data the class has as a formatted string to the `formatted_string` variable. Ideally each variable is it's own line in the `formatted_string`. For example, for `CMD_RTC_SYNC` we just add the unixtime of the board to the `formatted_string`. Remember to add the newline character so that the variables are formatted as their own lines.
```python
@dataclass
class CmdRtcSyncRes(CmdRes):
    """
    Child class for storing the response to the CMD_RTC_SYNC
    :param board_unixtime: The time on the board when the sync command was sent
    :type board_unixtime: int
    """

    board_unixtime: int

    def __str__(self) -> str:
        """
        Overriding the str method for a better representation of what's happening
        """
        formatted_string = super().__str__()
        # We just add the unixtime from the board into the string
        formatted_string += "Unixtime from Board: " + str(self.board_unixtime) + "\n"

        return formatted_string
```

### Step 5: Parsing Command Responses
If your command response has no data, you can skip this step. If your command response does have data we have to define a parsing callback function since the python side will receive the data as a array of bytes (or bytestring in python).

1. Locate the `command_response_callbacks.py`. Before the `parse_func_dict` we can define the function callback as seen in the code snipped below. Make sure to replace `CmdName`, `name`, `cmd_name` and `CMD_NAME` with the name of the command your parsing a response for.
```python
def parse_cmd_name(cmd_response: CmdRes, data: bytes) -> CmdCmdNameRes:
    """
    A function to parse the raw data from the response of CMD_NAME
    :param cmd_response: Basic command response
    :param data: The raw bytes containing the data that needs to be parsed
    :return: CmdCmdNameRes (i.e. A command response with no data for CMD_NAME)
    """
    if cmd_response.cmd_id != CmdCallbackId.CMD_NAME:
        raise ValueError("Wrong command id for parsing the name command")

    return CmdCmdNameRes(cmd_response.cmd_id, cmd_response.error_code, cmd_response.response_length)
```
2. In between the conditional and the return, add in the logic that takes a bytestring from the `data` parameter and converts it to data that can be stored in the command's respective response class. An example for the command response for `CMD_RTC_SYNC` is provided below.
```python
def parse_cmd_rtc_sync(cmd_response: CmdRes, data: bytes) -> CmdRtcSyncRes:
    """
    A function to parse the raw data from the response of CMD_RTC_SYNC
    :param cmd_response: Basic command response
    :param data: The raw bytes containing the data that needs to be parsed
    :return: CmdRes (i.e. A command response with no data for CMD_RTC_SYNC)
    """
    # TODO: Implement this callback properly
    if cmd_response.cmd_id != CmdCallbackId.CMD_RTC_SYNC:
        raise ValueError("Wrong command id for parsing the rtc sync command")

    # The first four bytes store an integer so we convert it back to an integer
    board_unixtime = int.from_bytes(data[:4], "little")

    # We use the data that we just created and construct the command response class for CMD_RTC_SYNC
    return CmdRtcSyncRes(cmd_response.cmd_id, cmd_response.error_code, cmd_response.response_length, board_unixtime)
```

3. Add the function callback you just created to the `parse_func_dict`, using the command enum in python as an index.
```python
parse_func_dict: dict[CmdCallbackId, Callable[..., CmdRes]] = defaultdict(lambda: parse_cmd_with_no_data)
parse_func_dict[CmdCallbackId.CMD_VERIFY_CRC] = parse_cmd_verify_crc
parse_func_dict[CmdCallbackId.CMD_RTC_SYNC] = parse_cmd_rtc_sync
parse_func_dict[CmdCallbackId.CMD_I2C_PROBE] = parse_cmd_i2c_probe
# Add entry here
```

### Step 6: Adding functions throughout to command utils
We need to add the command factory functions to a specific list in our command utils file so that everything works as intended.
1. Locate `command_utils.py`. In the `generate_commands` function, there will be a `command_factories` list. Add the command factory for your newly created command there.
```python
def generate_command(args: str) -> CmdMsg | None:
    """
    A function that parsed command arguments and returns the corresponding command frame

    :param args: The arguments to parse to create the command
    :return: CmdMsg structure with the requested command if the command is valid, else none
    """
    arguments = args.split()
    command = CmdMsg()

    # These are a list of parsers for commands that require additional arguments
    # NOTE: Update this list when another command with a specific parser is required
    child_parsers = [parse_cmd_downlink_logs_next_pass, parse_cmd_rtc_time_sync]

    # A list of Command factories for all commands
    # NOTE: Update these when a command is added and make sure to keep them in the order that the commands are described
    # in the CmdCallbackId Enum
    commmand_factories: list[Callable[..., CmdMsg]] = [
        create_cmd_end_of_frame,
        create_cmd_exec_obc_reset,
        create_cmd_rtc_sync,
        create_cmd_downlink_logs_next_pass,
        create_cmd_mirco_sd_format,
        create_cmd_ping,
        create_cmd_downlink_telem,
        create_cmd_uplink_disc,
        # Add command factory function here
    ]

```

## DONE!
That was a long list of steps that hopefully will be simplified in the future. But, you've successfully added your function. If something is wrong with the guide or something needs changing, bring it up on the discord!
