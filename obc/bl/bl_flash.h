#pragma once

#include <stdint.h>

uint32_t Fapi_Init(uint32_t startBank, uint32_t endBank);
uint32_t Fapi_BlockErase(uint32_t startAddr, uint32_t size);
uint32_t Fapi_BlockProgram(uint32_t flashAddress, uint32_t dataAddress, uint32_t numBytes);
