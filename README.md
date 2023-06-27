# OBC-firmware

This repository holds all the code that runs on our CubeSat's onboard computer (OBC).

## Table of Contents

- [Getting Started](#getting-started)
- [Contributing](#contributing)
- [Style Guide](#style-guide)
- [Authors](#authors)

**[Back to top](#table-of-contents)**

## Getting Started

This section will explain how to set up the repo, and how to build, flash, and debug the code.

### Dependencies

#### **Windows**
1. Download HALCoGen: https://www.ti.com/tool/HALCOGEN#downloads
    - This will be used for configuring the HAL. Unfortunately, the tool is only available on Windows.
2. Download UniFlash: https://www.ti.com/tool/UNIFLASH#downloads
    - This will be used for flashing the RM46.

3. Download Code Composer Studio (CCS): https://www.ti.com/tool/CCSTUDIO
    - This will be used for debugging.

4. Download WSL2: https://learn.microsoft.com/en-us/windows/wsl/install

5. In WSL2, run the following:
    ```sh
    sudo apt-get update
    sudo apt-get install build-essential
    ```
6. Choose the environment where you'll be running `git commit` (either WSL2 or the host). In that environment, install Python 3.8+ and pip if they're not already installed. Then, run the following in the OBC-firmware directory:
    ```sh
    pip install -r requirements.txt # You may want to create a Python virtual env before this
    pre-commit install
    ```
    **Note:** pre-commit is used to format your code whenever you make a commit.

You'll be using WSL2 primarily for building the firmware and running tests.

#### **MacOS**
Install required build tools (CMake, Make, gcc)
```sh
brew install cmake
brew install make
brew install gcc
```

Install Python 3.8+ and pip if they're not already installed, then run the following commands in the OBC-firmware directory:
```sh
pip install -r requirements.txt # You may want to create a Python virtual env before this
pre-commit install
```

Download UniFlash: https://www.ti.com/tool/UNIFLASH#downloads

Download Code Composer Studio (CCS): https://www.ti.com/tool/CCSTUDIO
#### **Linux**
Install required build tools (CMake, Make, gcc)
```sh
sudo apt-get update
sudo apt-get install build-essential
```
Download UniFlash: https://www.ti.com/tool/UNIFLASH#downloads

Download Code Composer Studio (CCS): https://www.ti.com/tool/CCSTUDIO

Install Python 3.8+ and pip if they're not already installed, then run the following commands in the OBC-firmware directory:
```sh
pip install -r requirements.txt # You may want to create a Python virtual env before this
pre-commit install
```

### Getting the Source

To clone this project and pull all required submodules, run the following:
```
git clone git@github.com:UWOrbital/OBC-firmware.git
git submodule update --init --recursive
```

### Building

#### **Firmware**
From the top-level directory, run the following to build the firmware.
```
mkdir build_arm && cd build_arm
cmake .. -DCMAKE_BUILD_TYPE=Firmware
cmake --build .
```
Take a look at `cmake/fw_build_options.cmake` to see the available build options.

#### **Tests**
From the top-level directory, run the following to build and run the tests.
```
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Test
cmake --build .
ctest --verbose
```

### Flashing
To flashing the RM46 (our microcontroller), we use Uniflash. Open Uniflash and select the appropriate device and connection.
#### **RM46 Launchpad:**
- Device = LAUNCHXL2-RM46
- Connection = Texas Instruments XDS110 USB Debug Probe

#### **OBC Revision 1/2:**
- Device = RM46L852
- Connection = Texas Instruments XDS110 USB Debug Probe

Then, click `Start` to begin a session. Select the `OBC-firmware.out` executable that you built (located in the `build_arm/` directory) and click `Load Image`. This will begin the flash process.

### Debugging
We use Code Composer Studio for debugging the firmware. **TODO**: Write a tutorial on how to use CCS.

## Contributing
1. Make sure you're added as a member to the UW Orbital organization on GitHub.
2. Create a feature branch for whatever task you're working on.
    * Our branch naming scheme is `<developer_name>/<feature_description>`.
      * Example: `danielg/implement-random-device-driver`
3. Make a PR.
    - For the PR description, make sure to fill in all the require details in the generated template.
    - Add at least 3 PR reviewers, including 1 firmware lead. When a PR is created, PR stats are added as a comment. You can use these stats to choose reviewers. Send a message in the #pr channel on Discord to notify the reviewers of your PR.
4. Make any requested changes and merge your branch onto main once the PR is approved.

**[Back to top](#table-of-contents)**

## Style Guide

### Comments

#### Single Line Comments

Variable and function names should be descriptive enough to understand even without comments. Comments are needed to describe any complicated logic. You may use `//` or `/* */` for single line comments.

#### Function Comments

Function comments should exist in the .h file. For static functions, they should exist in the .c file. Function comments should follow the format shown below:
```c
/**
 * @brief Adds two numbers together
 *
 * @param num1 - The first number to add.
 * @param num2 - The second number to add.
 * @return uint8_t - Returns the sum of of the two numbers.
 */
uint8_t addNumbers(uint8_t num1, uint8_t num2);
```

#### File Header Comments

- File comments are not required

#### Header Guard

- The symbol name should have the form `<PATH>_<FILE>_H_`

For example, if the file is `abc/xyz/foo.h`, then the header guard should be
```c
#ifndef ABC_XYZ_FOO_H_
#define ABC_XYZ_FOO_H_
...
#endif
```

### ****Naming and typing conventions****

-   `variableNames` in camelCase
-   `functionNames()` in camelCase
-   `#define MACRO_NAME` in CAPITAL_SNAKE_CASE
-   `file_names` in snake_case
-   `type_defs` in snake_case with _t suffix
    -   Ex:
        ```c
        typedef struct {
            int a;
            int b;
        } struct_name_t
        ```
-   4 spaces per level of indentation (NOT TABS)
-   Use spaces after opening brackets for conditionals and loops (e.g. `if ()` and `while ()`), but not for function calls (i.e. `my_func()`).
-   Import statments should be grouped in the following order:
    1.  Local imports (e.g. `#include "cc1120_driver.h`)
    2.  External library imports (e.g. `#include <os_semphr.h>`)
    3.  Standard library imports (e.g. `#include <stdint.h>`)
-   160 character limit per line (not a hard limit, use common sense)
-   Hanging indents should be aligned to delimeter:

```c
myFunction(hasToo,
            many, variables)
```

### ****General Rules****
Some of these rules don't apply in certain cases. Use your better judgement.

1. Avoid complex flow constructs, such as [goto](https://en.wikipedia.org/wiki/Goto) and [recursion](https://en.wikipedia.org/wiki/Recursion_(computer_science)).
2. All loops must have fixed bounds. This prevents runaway code.
3. Avoid [heap memory allocation](https://en.wikipedia.org/wiki/Memory_management#DYNAMIC).
4. Use an average of two [runtime assertions](https://en.wikipedia.org/wiki/Assertion_(software_development)#Assertions_for_run-time_checking) per function.
5. Restrict the scope of data to the smallest possible.
6. Check the return value of all non-void functions, or cast to void to indicate the return value is useless.
7. Limit pointer use to a single [dereference](https://en.wikipedia.org/wiki/Dereference_operator), and do not use [function pointers](https://en.wikipedia.org/wiki/Function_pointer).
8. Compile with all possible warnings active; all warnings should then be addressed before release of the software.

**[Back to top](#table-of-contents)**

## Authors
This repository was developed by the members of UW Orbital, the University of Waterloo's CubeSat design team.

**[Back to top](#table-of-contents)**
