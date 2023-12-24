#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "bl_errors.h"

#define BL_FLASH_ATTR_RAMFUNC_SECTION __attribute__((section(".ramFuncs")))

/**
 * @brief Initialize a flash bank
 *
 * @param bankNum The bank number to initialize
 * @return bl_error_code_t Error code
 */
bl_error_code_t bl_flash_fapiInitBank(uint32_t bankNum) BL_FLASH_ATTR_RAMFUNC_SECTION;

/**
 * @brief Erase flash sections
 *
 * @param startAddr Start address of the flash to erase
 * @param size Size of the flash to erase
 * @return bl_error_code_t Error code
 * @note Must initialize the bank first
 * @note All sectors with addresses in [startAddr, startAddr + size) will be erased
 */
bl_error_code_t bl_flash_fapiBlockErase(uint32_t startAddr, uint32_t size) BL_FLASH_ATTR_RAMFUNC_SECTION;

/**
 * @brief Write data to flash
 *
 * @param flashAddress The address to write to
 * @param dataAddress The address of the data to write
 * @param numBytes The number of bytes to write
 * @return bl_error_code_t Error code
 */
bl_error_code_t bl_flash_fapiBlockWrite(uint32_t flashAddress, uint32_t dataAddress,
                                        uint32_t numBytes) BL_FLASH_ATTR_RAMFUNC_SECTION;

/**
 * @brief Check if an address is a valid start address for the binary
 *
 * @param addr The address to check
 * @param binSize The size of the binary
 * @return true if the address is valid, false otherwise
 */
bool bl_flash_isStartAddrValid(uint32_t addr, uint32_t binSize) BL_FLASH_ATTR_RAMFUNC_SECTION;

/**
 * @brief Wait for the flash state machine to be ready
 *
 */
void bl_flash_waitFsmReady(void) BL_FLASH_ATTR_RAMFUNC_SECTION;

/**
 * @brief Wait for the flash state machine status to be success
 *
 */
void bl_flash_waitFsmStatusSuccess(void) BL_FLASH_ATTR_RAMFUNC_SECTION;

/**
 * @brief Get the sector number of an address
 *
 * @param addr The address to get the sector number of
 * @return uint8_t The sector number
 */
uint8_t bl_flash_sectorOfAddr(uint32_t addr) BL_FLASH_ATTR_RAMFUNC_SECTION;

/**
 * @brief Get the start address of a sector
 *
 * @param sector The sector number
 * @return uint32_t The start address of the sector
 */
uint32_t bl_flash_sectorStartAddr(uint8_t sector) BL_FLASH_ATTR_RAMFUNC_SECTION;

/**
 * @brief Get the end address of a sector
 *
 * @param sector The sector number
 * @return uint32_t The end address of the sector
 */
uint32_t bl_flash_sectorEndAddr(uint8_t sector) BL_FLASH_ATTR_RAMFUNC_SECTION;

/**
 * @brief Get the number of sectors
 *
 * @return uint8_t The number of sectors
 */
uint8_t bl_flash_getNumSectors(void) BL_FLASH_ATTR_RAMFUNC_SECTION;
