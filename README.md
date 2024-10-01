# OBC-firmware

This repository holds all the code written by UW Orbital's firmware team. This includes: OBC firmware/embedded software, ground station software, and all testing infrastructure.

## Table of Contents

- [Getting Started](#getting-started)
- [Contributing](#contributing)
- [C Style Guide](#c-style-guide)
- [Python Style Guide](#python-style-guide)
- [Pytest Style Guide](#pytest-style-guide)
- [Authors](#authors)

**[Back to top](#table-of-contents)**

## Getting Started

This section will explain how to set up the repo, and how to build, flash, and debug the code.

### Getting the Source

1. Check if you have Git installed on your system by running `git --version` in a terminal. If it returns some version number, then it's installed. If not, follow the installation steps listed [here](https://git-scm.com/book/en/v2/Getting-Started-Installing-Git). If you're on Windows, it's highly recommended that you also install Git Bash and use that instead of the command prompt or Powershell.
2. To clone this project, run the following command in whatever directory you want to store the repository in:
```
git clone git@github.com:UWOrbital/OBC-firmware.git
```

### Dependencies

#### HALCoGen

Download HALCoGen here: https://www.ti.com/tool/HALCOGEN#downloads. This will be used for configuring the HAL. Unfortunately, the tool is only available on Windows. If you're on a non-Windows system, you may be able to set HALCoGen up in a Windows VM or someone else on the team can make HAL changes for you. We don't have to modify the HAL very often anyways.

#### Code Composer Studio (CCS)

Download Code Composer Studio (CCS): https://www.ti.com/tool/CCSTUDIO. This will be used for debugging.

#### Uniflash

Download UniFlash here: https://www.ti.com/tool/UNIFLASH#downloads. This will be used for flashing the RM46.

#### Docker Development Environment

It's **highly recommended** that you set up your development environment using Docker and VSCode, especially if you're new to software development. If you follow the instructions in this section, you can skip the **Windows/MacOS/Linux** sections. If you know what you're doing, feel free to set up your dev environment however you like using the instructions in the **Windows/MacOS/Linux** sections for reference. However, there may be a lack of support from other leads/members who don't use the same setup.

##### Docker Desktop Installation & Configuration
1. Install Docker Desktop App from [this link](https://www.docker.com/products/docker-desktop/)
    - You can choose to sign-up/create an account but it's not required. You can also skip the "Tell-us about what you do" section.
2. Open Docker Desktop and click on ```Dev Environments``` from the side-panel
    - Click on create on ```Create +``` in the top-right corner.
3. Setting up the repo
    - Name the Environment as desired
    - For the ```Choose source``` option, select ```Local directory``` and then select the `OBC-firmware` repository folder that you cloned earlier.
    - Click ```Continue```
    - Once the container is created, you should be able to open the container in VSCode. If you have VSCode, you can press ```Open in VSCode```. If you don't have VSCode, you can get it here: https://code.visualstudio.com/download

##### Installing Dependencies
Once you open the docker instance, open a terminal in VSCode and run the following commands. The dollar sign in your terminal should be prefaced by something like this: ```root ➜ /com.docker.devenvironments.code (main ✗)```.

This command opens a terminal in VSCode: ```Ctrl + Shift + ` ```

Enter these commands in your terminal:
```
sudo apt-get update
sudo apt-get install -y python3-pip build-essential cmake
pip3 install -r requirements.txt
pre-commit install
```

##### Testing The Container Build

To test whether your Dev environment has been set up correctly run the commands in the **Building** section. The OBC firmware and test builds should pass. All tests should succeed.

**Note**: The docker container uses pre-configured git (one added to the original OS path by the user). So you should be able to pull and push to the OBC repo as necessary.

**Tip**: Use the ```git config --list``` command on the VsCode terminal to confirm your git info.

#### **Windows**
Skip this section if you set up a Docker development environment.

1. Download WSL2: https://learn.microsoft.com/en-us/windows/wsl/install

2. In WSL2, run the following:
    ```sh
    sudo apt-get update
    sudo apt-get install build-essential
    ```
3. Choose the environment where you'll be running `git commit` (either WSL2 or the host) and install Python 3.10 and pip. (Only required for Python devs)
   A. If using WSL, follow the instructions under the `Linux` section 2.

   B. If you are using Windows. Run the following commands in the OBC-firmware directory:

   Install Python 3.10.12: https://www.python.org/downloads/release/python-31012/
   ```sh
   C:\path\to\python\executable -m venv .venv
   .\Scripts\bin\activate
   pip install -r requirements.txt
   pip install -e .
   ```

4. Setup pre-commit.
    In the WSL, under the OBC-firmware directory, run the following commands:
    ```sh
    pip install -r requirements.txt # You may want to create a Python virtual env before this if you haven't already
    pre-commit install
    ```
    - You may receive a message in yellow saying where pre-commit.exe was installed and that you need to add it to PATH
        - To do this go to View advanced System settings -> Environment Variables -> Path -> Edit and click new to paste the path to where pre-commit.exe is                    installed into here. You may need to restart after doing this for the changes to take place.
    - Once your PATH is set up and pre-commit is installed you can use `pre-commit run --all-files` to format all of your files before committing
    **Note:** pre-commit is used to format your code whenever you make a commit.

You'll be using WSL2 primarily for building the firmware and running tests.

#### **MacOS**
Skip this section if you set up a Docker development environment.

1. Install required build tools (CMake, Make, gcc)
```sh
brew install cmake
brew install make
brew install gcc
```

2. Install Python 3.10 and setup Python virtual environment (Only required for Python devs)

Run the following commands in the OBC-firmware directory:
```sh
brew install python@3.10
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
pip install -e .
```

3. Setup pre-commit
```sh
pip install -r requirements.txt # You may want to create a Python virtual env before this if you haven't already
pre-commit install
```

#### **Linux**
Skip this section if you set up a Docker development environment.

1. Install required build tools (CMake, Make, gcc)
```sh
sudo apt-get update
sudo apt-get install build-essential
```

2. Install Python 3.10 and setup Python virtual environment (Only required for Python devs)

Run the following commands in the OBC-firmware directory:
```sh
sudo apt-get install python3.10
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
pip install -e .
```

3. Setup pre-commit
```sh
pip install -r requirements.txt # You may want to create a Python virtual env before this if you haven't already
pre-commit install
```

### Building

#### **OBC Firmware**
From the top-level directory, run the following to build the OBC firmware.
```
mkdir build_arm && cd build_arm
cmake .. -DCMAKE_BUILD_TYPE=OBC
cmake --build .
```
Take a look at `cmake/fw_build_options.cmake` to see the available build options.

#### **Ground Station**
From the top-level directory, run the following to build the ground station. Currently, the ground station is only supported on Windows.
```
mkdir build_gs && cd build_gs
cmake .. -DCMAKE_BUILD_TYPE=GS
cmake --build .
```

#### **Tests**
From the top-level directory, run the following to build and run the tests.
```
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Test
cmake --build .
ctest --verbose
```

### Flashing
To flash the RM46 (our microcontroller), we use Uniflash. Open Uniflash and select the appropriate device and connection.
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
    - For the PR description, make sure to fill in all the required details in the generated template.
    - Add at least 3 PR reviewers, including 1 firmware lead. When a PR is created, PR stats are added as a comment. You can use these stats to choose reviewers. Send a message in the #pr channel on Discord to notify the reviewers of your PR.
4. Make any requested changes and merge your branch onto main once the PR is approved.

**[Back to top](#table-of-contents)**

## C Style Guide

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
 * @return Returns the sum of the two numbers.
 */
uint8_t addNumbers(uint8_t num1, uint8_t num2);
```

#### File Header Comments

- File comments are not required

#### Header Guard

We use `#pragma once` instead of include guards.

### ****Naming and typing conventions****

-   `variableNames` in camelCase
-   `functionNames()` in camelCase
-   `#define MACRO_NAME` in CAPITAL_SNAKE_CASE
-   `file_names` in snake_case
-   `type_defs` in snake_case with _t suffix
    -   Ex:
        ```c
        typedef struct {
          uint32_t a;
          uint32_t b;
        } struct_name_t
        ```
-   Import statements should be grouped in the following order:
    1.  Local imports (e.g. `#include "cc1120_driver.h`)
    2.  External library imports (e.g. `#include <os_semphr.h>`)
    3.  Standard library imports (e.g. `#include <stdint.h>`)

### ****General Rules****
Some of these rules don't apply in certain cases. Use your better judgement. To learn more about these rules, research NASA's Power of 10.

1. Avoid complex flow constructs, such as [goto](https://en.wikipedia.org/wiki/Goto) and [recursion](https://en.wikipedia.org/wiki/Recursion_(computer_science)).
2. All loops must have fixed bounds. This prevents runaway code.
3. Avoid [heap memory allocation](https://en.wikipedia.org/wiki/Memory_management#DYNAMIC).
4. Use an average of two [runtime assertions](https://en.wikipedia.org/wiki/Assertion_(software_development)#Assertions_for_run-time_checking) per function.
5. Restrict the scope of data to the smallest possible.
6. Check the return value of all non-void functions, or cast to void to indicate the return value is useless.
7. Limit pointer use to a single [dereference](https://en.wikipedia.org/wiki/Dereference_operator), and do not use [function pointers](https://en.wikipedia.org/wiki/Function_pointer).
8. Compile with all possible warnings active; all warnings should then be addressed before release of the software.
9. Use the preprocessor sparingly
10. Restrict functions to a single printed page

**[Back to top](#table-of-contents)**

## Python Style Guide

- We will be following the Python language style guide [PEP8](https://peps.python.org/pep-0008/)
- If there are any discrepancies between this style guide and PEP8, this style guide takes precedence.

### Type Hinting Convention

All function and method parameters (except for the `self` and `cls` parameters) and return signatures should be type hinted.

```python
def my_add(num1: int, num2: int) -> int:
	"""
	@brief Adds two numbers together

	@param num1 - The first number to add.
	@param num2 - The second number to add.
	@return Returns the sum of the two numbers.
	"""
	return num1 + num2
```

### Comments

#### Single Line Comments

Variable and function names should be descriptive enough to understand even without comments. Comments are needed to describe any complicated logic. Use `#` for single-line comments.

#### Function and Method Comments

Function and method comments using `""" """` should exist below the function declaration. For methods, the `self` or `cls` parameter does not require a description.

```python
def my_add(num1: int, num2: int) -> int:
	"""
	@brief Adds two numbers together

	@param num1 - The first number to add.
	@param num2 - The second number to add.
	@return Returns the sum of the two numbers.
	"""
	return num1 + num2
```

```python
def increase_x(self, count: int) -> None:
	"""
	@brief Increases the x attribute by the count.

	@param count - Count to increase the x attribute by.
	"""
	self.x += count
```

#### File Header Comments

File comments are not required

#### Class Comments

- Class comments should exist after the class definition
- Provide a brief description given class purpose
- Provide a section in the class comment listing the attributes, their type and purpose
- Enum class comments do not require listing the attributes

```python
class PointTwoDimension:
	"""
	@brief Class for storing a 2D point
	@attribute x (int) - x coordinate of the point
	@attribute y (int) - y coordinate of the point
	"""

	def __init__(x: int, y: int):
		self.x = x
		self.y = y

@dataclasses.dataclass
class PointTwoDimension:
	"""
	@brief Class for storing a 2D point
	@attribute x (int) - x coordinate of the point
	@attribute y (int) - y coordinate of the point
	"""

	x: int
	y: int
```
 ```python
import enum

# No comments required
class ErrorCode(enum.Enum):
    """
    @brief Enum for the error codes
    """

    SUCCESS = 0
    INVALID_ARG = 1
```

### Naming Conventions

- `variable_names`, `field_names` and `function_constants` in snake_case
- `_private_field_names`, and `_private_method_names()` in \_snake_case
- `function_names()` and `method_names()` in snake_case
- `CONSTANT_NAMES: Final` and `ENUM_OPTIONS` in CAPITAL_SNAKE_CASE for module and class constants (not for local constant)
- `file_names` in snake_case
- `ClassName` in PascalCase
    ```python
    # For brevity, the class comments were removed but they should be in real code
    import dataclasses

    @dataclasses.dataclass
    class PointTwoDimension:
    	x: int
    	y: int

    class PointTwoDimension:
    	def __init__(x: int, y: int):
    		self.x = x
    		self.y = y
    ```


- `EnumName` in PascalCase

    ```python
    import enum

    class ErrorCode(enum.Enum):
    	SUCCESS = 0
    	INVALID_ARG = 1

    # Accessing:
    ErrorCode.SUCCESS  # <ErrorCode.SUCCESS: 0>
    ErrorCode.INVALID_ARG  # <ErrorCode.INVALID_ARG: 1>
    ```

### Imports

#### Grouping Imports

Handled by pre-commit

#### Notes about imports

- Imports should only be used at the top of the file (no function or scoped imports)
- Only modules should be imported

```python
# module1 contains very_long_module_name and function foo and variable var.
#   very_long_module_name contains bar

# Yes:
from module1 import very_long_module_name as module2  # Casting to shorter name
import module1

module1.foo()
module1.var
module2.bar()

# No:
from module1.very_long_module_name import bar
from module1 import foo, var

foo()
var
bar()
```

### Other Style Guide Points

- Only imports, function, class, and constants declarations and the `if __name__ == '__main__'` should be in module scope
- Entry point to a script or program should be through the `main` function
- Add a trailing comma after elements of a list, if you wish to make/preserve each element on a separate line

**[Back to top](#table-of-contents)**

## Pytest Style Guide

- All functions that are to be tested should go into a Python file starting with `test_`
- All functions that are to be tested **must** start with `test_` (This is a Pytest requirement)
- Type hints are optional for Pytest functions (functions that start with `test_` in a Pytest file)

**[Back to top](#table-of-contents)**

## Authors
This codebase was developed by the members of UW Orbital, the University of Waterloo's CubeSat design team.

**[Back to top](#table-of-contents)**
