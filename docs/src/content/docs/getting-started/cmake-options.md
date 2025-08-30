---
title: CMake options guide
description: Some useful options and options that we have defined
---
The following is a list of options along with their brief description. The guide is divided into most used and extra for convenience. You can go to the end of this page for examples on how to chain these options together.

:::note
**Options suffixed with an equal sign should have an argument passed into them.** Options without an equal sign suffix just need to be specified, no arguments needed.

If an argument is marked as **Default**, it will be the default argument that is used if the option is not specified. If there are no arguments marked as default then nothing will happen.
:::

## Most used options
These are options that you will frequently be using while compiling. **Unless otherwise specified these options are to be use within the `build_arm` directory of the project**

### `-DCMAKE_BUILD_TYPE=` (can be used in directories other than `build_arm`)
This specifies what the current build directory is for. You can pass either of the **four** arguments into this command...
* `OBC`: Used to build for the board (use in the `build_arm` directory)
* `GS`: Used to build code for the ground station (use in the `build_gs` directory)
* `Test`: Used to build the testing directory for the interfaces (use in the `build` folder)
* `Examples`: Used to build examples which are barebone binaries to test certain functionality on the board (use in the `build_examples` directory)

### `-DBOARD_TYPE=`
This option specifies the board that the binary is being built for since different board files will require different HAL (Hardware Abstraction Layer) files to be sourced. You can pass either of the **three** arguments...
* **Default** `RM46_LAUNCHPAD`: Build the files for the launchpad board
* `OBC_REVISION_1`: Build the files for the first revision of the OBC
* `OBC_REVISION_2`: Build the files for the second revision of the OBC


## Extra options

### `-DDEBUG`
If specified, this removes all optimization flags to debug. Sometimes optimizations can interfere with critical code but, first check your code's logic before using this option.

### `-DLOG_DEFAULT_LEVEL=`
Specifies the default level for logs. You can pass any enum in the `obc_logging.h` file to this option.

### `-DENABLE_BL_BYPASS=`
A boolean (takes a 0 or a 1) that specifies whether the app should bypass the bootloader (when you want to flash without the bootloader) to load the app. The boolean values define the following functionality...
* `0`: The app should not bypass the bootloader and write to the `0x0` address (this is the default address the board looks for to load a program).
* **Default** `1`: The app should bypass the bootloader and write to `0x0`

### `-DCOMMS_PHY=`
:::tip[Warning]{icon="seti:html"}
This option is not fully implemented yet. It's for when comms is complete.
:::

### `-DOBC_UART_BAUD_RATE=`
This option helps set the baud rate for the board.
:::caution
When using this option, you have to change the baud rate in the HAL using **`HALCOGen`**
:::
You can pass any **valid integer baud rate** into this option.

### `-DENABLE_TASK_STATS_COLLECTOR`
This option enables or disables the task stats collector (statistics on how much memory/cpu each task is using sent via logs). This is a boolean option with each argument defining the functionality...
* `0`: The app should not send task statistics via logs
* **Default** `1`: The app should send task statistics via logs

### `-DCMAKE_EXPORT_COMPILE_COMMANDS=ON`
This is a useful option for IDEs like `neovim` that use Language Servers that require a `compile_commands.json` file. If specified this command will generate a `compile_commands.json` file, else it will not.
:::tip
By default cmake will generate this file in the directory you use the `cmake ..` command in. You may have to create a symbolic link to a `compile_commands.json` file in your root directory.
:::

## Examples
The following are some examples that might be useful in understanding how to define these options.

1. Say you wanted to build for the 1st revision of the obc and wanted to use the bootloader.
    ```shell
    cmake .. -DCMAKE_BUILD_TYPE=OBC -DBOARD_TYPE=OBC_REVISION_1 -DENABLE_BL_BYPASS=0
    ```

2. Say you wanted to build for the 2nd revision of the obc and wanted to output compile commands as well.
    ```shell
    cmake .. -DCMAKE_BUILD_TYPE=OBC -DBOARD_TYPE=OBC_REVISION_2 -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    ```
