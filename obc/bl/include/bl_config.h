#pragma once

#include <stdint.h>

#define CRYSTAL_FREQ 16UL   // MHz
#define SYS_CLK_FREQ 220UL  // MHz

//*****************************************************************************
// The starting address of the application. This must be aligned to a sector
// boundary and be outside the bootloader's address space.
//
// The flash image of the bootloader must not be larger than this value.
//*****************************************************************************
#define APP_START_ADDRESS (uint32_t)0x00040000
