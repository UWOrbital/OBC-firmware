# OBC-firmware

This repository holds all the code written by UW Orbital's software team. This includes OBC firmware/embedded software, ground station software, and all testing infrastructure.

## Table of Contents

- [UW Orbital Docs](#uw-orbital-docs)
- [Firmware Dependencies](#firmware-dependencies)
- [Contributing](#contributing)
- [C Style Guide](#c-style-guide)
- [Python Style Guide](#python-style-guide)
- [Pytest Style Guide](#pytest-style-guide)
- [TypeScript/React Style Guide](#typescriptreact-style-guide)
- [Authors](#authors)

## UW Orbital Docs
The UW Orbital Docs are a great source of codebase documentation. The site includes many detailed setup guides as well as the style guides below. We highly recommend following these guides if you are new to the team or software development in general.

Check out the site here: https://uworbital.github.io/OBC-firmware/

## Firmware Dependencies

#### HALCoGen

Download HALCoGen here: https://www.ti.com/tool/HALCOGEN#downloads. This will be used for configuring the HAL. Unfortunately, the tool is only available on Windows. If you're on a non-Windows system, you may be able to set HALCoGen up in a Windows VM or someone else on the team can make HAL changes for you. We don't have to modify the HAL very often anyways.

#### Code Composer Studio (CCS)

Download Code Composer Studio (CCS): https://www.ti.com/tool/CCSTUDIO. This will be used for debugging.

#### Uniflash

Download UniFlash here: https://www.ti.com/tool/UNIFLASH#downloads. This will be used for flashing the RM46.

## Contributing

1. Make sure you're added as a member to the UW Orbital organization on GitHub.
2. Create a feature branch for whatever task you're working on.
   - Our branch naming scheme is `<developer_name>/<feature_description>`.
     - Example: `danielg/implement-random-device-driver`
3. Make a PR.
   - For the PR description, make sure to fill in all the required details in the generated template.
   - Add at least 3 PR reviewers, including 1 software lead. When a PR is created, PR stats are added as a comment. You can use these stats to choose reviewers. Send a message in the #pr channel on Discord to notify the reviewers of your PR.
4. Make any requested changes and merge your branch onto main once the PR is approved.

## C Style Guide

### Comments

#### Single line comments

Variable and function names should be descriptive enough to understand even without comments. Comments are needed to describe any complicated logic. You may use `//` or `/* */` for single line comments.

#### Function comments

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

#### File header comments

File comments are not required.

#### Header guards

We use `#pragma once` instead of include guards.

### Naming and typing conventions

- `variableNames` in camelCase
- `functionNames()` in camelCase
- `#define MACRO_NAME` in CAPITAL_SNAKE_CASE
- `file_names` in snake_case
- `type_defs` in snake_case with \_t suffix
  - Ex:
    ```c
    typedef struct {
      uint32_t a;
      uint32_t b;
    } struct_name_t
    ```
- Import statements should be grouped in the following order:
  1.  Local imports (e.g. `#include "cc1120_driver.h"`)
  2.  External library imports (e.g. `#include <os_semphr.h>`)
  3.  Standard library imports (e.g. `#include <stdint.h>`)

### General rules

Some of these rules don't apply in certain cases. Use your better judgement. To learn more about these rules, research NASA's Power of 10.

1. Avoid complex flow constructs, such as [goto](https://en.wikipedia.org/wiki/Goto) and [recursion](<https://en.wikipedia.org/wiki/Recursion_(computer_science)>).
2. All loops must have fixed bounds. This prevents runaway code.
3. Avoid [heap memory allocation](https://en.wikipedia.org/wiki/Memory_management#DYNAMIC).
4. Use an average of two [runtime assertions](<https://en.wikipedia.org/wiki/Assertion_(software_development)#Assertions_for_run-time_checking>) per function.
5. Restrict the scope of data to the smallest possible.
6. Check the return value of all non-void functions, or cast to void to indicate the return value is useless.
7. Limit pointer use to a single [dereference](https://en.wikipedia.org/wiki/Dereference_operator), and do not use [function pointers](https://en.wikipedia.org/wiki/Function_pointer).
8. Compile with all possible warnings active; all warnings should then be addressed before release of the software.
9. Use the preprocessor sparingly.
10. Restrict functions to a single printed page.

## Python Style Guide

- We will be following the Python language style guide [PEP8](https://peps.python.org/pep-0008/)
- If there are any discrepancies between this style guide and PEP8, this style guide takes precedence.

### Type hinting convention

All function and method parameters (except for the `self` and `cls` parameters) and return signatures should be type hinted.

```python
def my_add(num1: int, num2: int) -> int:
	"""
	Adds two numbers together

  	:warning: Add a warning if your function requires
  	:note: Add a note that other developers might find helpful

	:param num1: The first number to add.
	:param num2: The second number to add.
	:return: Returns the sum of the two numbers.
	"""
	return num1 + num2
```

### Comments

#### Single line comments

Variable and function names should be descriptive enough to understand even without comments. Comments are needed to describe any complicated logic. Use `#` for single-line comments.

#### Function and method comments

Function and method comments using `""" """` should exist below the function declaration. For methods, the `self` or `cls` parameter does not require a description.

```python
def my_add(num1: int, num2: int) -> int:
	"""
	Adds two numbers together

  	:warning: Add a warning if your function requires
  	:note: Add a note that other developers might find helpful

	:param num1: The first number to add.
	:param num2: The second number to add.
	:return: Returns the sum of the two numbers.
	"""
	return num1 + num2
```

Notice that the docstring is formatted using reST (reStructuredText) with two outliers: `:warning:` and `:note:`. The outliers will need to be changed to their proper, `..note::` and `..warning::` counterparts if doc-generation using sphinx is implemented.

```python
def increase_x(self, count: int) -> None:
	"""
	Increases the x attribute by the count.

	:param count: Count to increase the x attribute by.
	"""
	self.x += count
```

#### File header comments

File comments are not required.

#### Class comments

- Class comments should exist after the class definition
- Provide a brief description given class purpose
- Provide a section in the class comment listing the attributes, their type and purpose
- Enum class comments do not require listing the attributes

```python
class PointTwoDimension:
	"""
	Class for storing a 2D point

	:param x: x coordinate of the point
	:type x: int
	:param y: y coordinate of the point
	:type y: int
	"""

	def __init__(x: int, y: int):
		self.x = x
		self.y = y

@dataclass
class PointTwoDimension:
	"""
	Class for storing a 2D point

	:param x: x coordinate of the point
	:type x: int
	:param y: y coordinate of the point
	:type y: int
	"""

	x: int
	y: int
```

```python
from enum import Enum

# No comments required
class ErrorCode(Enum):
   """
   Enum for the error codes
   """

   SUCCESS = 0
   INVALID_ARG = 1
```

### Naming conventions

- `variable_names`, `field_names` and `function_constants` in snake_case
- `_private_field_names`, and `_private_method_names()` in \_snake_case
- `function_names()` and `method_names()` in snake_case
- `CONSTANT_NAMES: Final` and `ENUM_OPTIONS` in CAPITAL_SNAKE_CASE for module and class constants (not for local constant)
- `file_names` in snake_case
- `ClassName` in PascalCase

  ```python
  # For brevity, the class comments were removed but they should be in real code
  from dataclasses import dataclass

  @dataclass
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
  from enum import Enum

  class ErrorCode(Enum):
  	SUCCESS = 0
  	INVALID_ARG = 1

  # Accessing:
  ErrorCode.SUCCESS  # <ErrorCode.SUCCESS: 0>
  ErrorCode.INVALID_ARG  # <ErrorCode.INVALID_ARG: 1>
  ```

### Imports

#### Grouping imports

This is handled by pre-commit.

#### Notes about imports

- Imports should only be used at the top of the file (no function or scoped imports)
- Modules should not be imported

```python
# module1 contains very_long_module_name and function foo and variable var.
#   very_long_module_name contains bar

# No:
from module1 import very_long_module_name as module2
import module1

module1.foo()
module1.var
module2.bar()

# Yes:
from module1.very_long_module_name import bar
from module1 import foo, var

foo()
var
bar()
```

### Other style guide points

- Only imports, function, class, and constants declarations and the `if __name__ == '__main__'` should be in module scope
- Entry point to a script or program should be through the `main` function
- Add a trailing comma after elements of a list, if you wish to make/preserve each element on a separate line

## Pytest Style Guide

- All functions that are to be tested should go into a Python file starting with `test_`
- All functions that are to be tested **must** start with `test_` (This is a Pytest requirement)
- Type hints are optional for Pytest functions (functions that start with `test_` in a Pytest file)

## TypeScript/React Style Guide

### Comments

#### Single line comments

Variable and function names should be descriptive enough to understand even without comments. Comments are needed to describe any complicated logic. You may use `//` or `/* */` for single line comments.

#### Function comments

Function comments should follow the format shown below:

```typescript
/**
 * @brief Adds two numbers together
 *
 * @param num1 - The first number to add.
 * @param num2 - The second number to add.
 * @return Returns the sum of the two numbers.
 */
function addNumbers(num1: number, num2: number): number {
  return num1 + num2;
}
```

#### File comments

File comments are not required.

### Naming and typing conventions

-   `variableNames` in camelCase
-   `functionNames()` in camelCase
-   `CONSTANT_NAME` in SCREAMING_SNAKE_CASE
-   `file-names` in kebab-case
-   `ClassName` and `ComponentName` in PascalCase

## Authors

This codebase was developed by the members of UW Orbital, the University of Waterloo's CubeSat design team.
