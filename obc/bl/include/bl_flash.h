#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "bl_errors.h"

#define BL_FLASH_API_SECTION __attribute__((section(".flashApi")))

bl_error_code_t bl_flash_FapiInitBank(uint32_t bankNum) BL_FLASH_API_SECTION;
bl_error_code_t bl_flash_FapiBlockErase(uint32_t startAddr, uint32_t size) BL_FLASH_API_SECTION;
bl_error_code_t bl_flash_FapiBlockWrite(uint32_t flashAddress, uint32_t dataAddress,
                                        uint32_t numBytes) BL_FLASH_API_SECTION;
bool bl_flash_isStartAddrValid(uint32_t addr, uint32_t binSize) BL_FLASH_API_SECTION;
void bl_flash_waitFsmReady(void) BL_FLASH_API_SECTION;
void bl_flash_waitFsmStatusSuccess(void) BL_FLASH_API_SECTION;
