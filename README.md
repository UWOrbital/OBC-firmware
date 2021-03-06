# OBC-firmware

This repository holds all the code that runs on our CubeSat's onboard computer (OBC).

## Table of Contents

1. [About the Project](#about-the-project)
1. [Getting Started](#getting-started)
    1. [Dependencies](#dependencies)
    2. [Setup](#getting-the-source)
    3. [Building](#building)
    4. [Flashing and Debugging](#flashing-and-debugging)
1. [Contributing](#contributing)
1. [Style Guide](#style-guide)
1. [Authors](#authors)

## About the Project

* `adcs/`
    * All code for the attitude determination and control system subsystem.
* `cdh/`
    * All code for the command and data handling subsystem.
* `comms/`
    * All code for the communications subsystem.
* `hal/`
    * The hardware abstraction layer generated by HALCoGen.
* `payload/`
    * All code for the primary and secondary payload.

**[Back to top](#table-of-contents)**

## Getting Started

This section will explain how to set up the repo, and how to build, flash, and debug the code.

### Dependencies

The following software should be installed:
* GCC ARM Embedded Toolchain
* UniFlash
* OpenOCD
* Code Composer Studio

**Instructions on how to install these tools can be found on [this Notion page.](https://www.notion.so/uworbital/OBC-Firmware-Development-Workflow-ab037261ce6c45189ea5ca8486b02c6b)**

### Getting the Source

This project is [hosted on GitHub](https://github.com/UWOrbital/OBC-firmware). You can clone this project directly using this command:
```
git clone git@github.com:UWOrbital/OBC-firmware.git
```

### Building

You can build the project using these commands at the top-level of the repo:

```
make clean # Delete any previous build files
make # Build the .out file. It should appear in the build directory.
```
If you get a main() already defined error, go remove the file hal/source/sys_main.c.

More information can be found on [this Notion page.](https://www.notion.so/uworbital/OBC-Firmware-Development-Workflow-ab037261ce6c45189ea5ca8486b02c6b)

### Flashing and Debugging
Information about flashing the device and debugging can be found on [this Notion page.](https://www.notion.so/uworbital/OBC-Firmware-Development-Workflow-ab037261ce6c45189ea5ca8486b02c6b)

## Contributing
1. Make sure you're added as a member to the UW Orbital organization on GitHub.
2. Create a feature branch for whatever task you're working on.
    * Our branch naming scheme is `<subteam>/<developer_name>/<feature_description>`. Ignore the `<developer_name>` part if the branch has multiple developers.
    * Example: `cdh/daniel/implement-random-device-driver`
    * Another example: `cdh/implement-random-device-driver`
3. Make a PR. Make sure to at least add the CDH leads as reviewers and ping the CDH pr channel on Discord. You may also want to add your subteam lead(s) as a reviewer if you're not on CDH.
    * Pull requests should include information on, at minimum, the purpose of the PR, new changes made in the PR, tests performed to verify that the code works, and changes that can be made in future iterations of the feature. See [this sample template](https://github.com/UWOrbital/CC1120Driver/blob/main/.github/pull_request_template.md) for more. It???s good practice to have a PR template in a .github folder in every repository you create. GitHub will pull from this template every time a new PR is made.
5. Make any requested changes and merge your branch onto main once the PR is approved.

**[Back to top](#table-of-contents)**

## Style Guide

### Comments

#### Single Line Comments

Variable and function names should be descriptive enough to understand even without comments, but if comments are needed to explain parts of the code, use single line comments with the `/* */` notation for better compatibility with old compilers in C.
```c
/* Single line comment */
// Do not use this for C code
```
#### Function Comments

Function comments should exist in both the .h and .c files optimally, but at minimum they should be available in the .h files. Comments should follow the format shown below:
```c
/**
 * @brief Adds two numbers together
 * 
 * @param num1 - The first number to add.
 * @param num2 - The second number to add.
 * @return uint8_t - Returns the sum of of the two numbers.
 */
uint8_t add_numbers(uint8_t num1, uint8_t num2);
```

#### File Header Comments

-   File comments are not required

### ****Naming and typing conventions****

-   `variableNames` in camelCase
-   `function_names()` in snake_case
-   `ClassNames` in PascalCase (UpperCamelCase)
-   `CONSTANT_NAMES` in CAPITAL_SNAKE_CASE
-   `file_names` in snake_case
-   4 spaces per level of indentation
-   Use spaces after opening brackets for conditionals and loops (e.g. `if ()` and `while ()`), but not for function calls (i.e. `my_func()`).
-   Operators:
    -   No spaces around `*`, `/`, `%`, `!`
    -   One space on either side of `=`, `==`, `+`, `-`, `+=`, `-=`, etc
    -   One space after every comma `my_func(var1, var2, var3)`
-   Import statments should be grouped in the following order:
    1.  Local imports (e.g. `#include "cc1120_driver.h`)
    2.  External library imports (e.g. `#include <semphr.h>`)
    3.  Standard library imports (e.g. `#include <stdint.h>`)
-   160 character limit per line (not a hard limit, use common sense)
-   Hanging indents should be aligned to delimeter:

```c
my_function(hasToo,
            many, variables)
```
**[Back to top](#table-of-contents)**

## Authors
This repository was developed by the members of UW Orbital, the University of Waterloo's CubeSat design team.

**[Back to top](#table-of-contents)**
