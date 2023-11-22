#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "bl_errors.h"

bl_error_code_t bl_flash_FapiInitBank(uint32_t bankNum);
bl_error_code_t bl_flash_FapiBlockErase(uint32_t startAddr, uint32_t size);
bl_error_code_t bl_flash_FapiBlockWrite(uint32_t flashAddress, uint32_t dataAddress, uint32_t numBytes);
bool bl_flash_isStartAddrValid(uint32_t addr, uint32_t binSize);
