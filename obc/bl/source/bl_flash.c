#include "F021.h"
#include "flash_config.h"
#include "bl_config.h"
#include "bl_flash.h"
#include "bl_errors.h"
#include "sci.h"
#include <stdint.h>

/* PRIVATE FUNCTION DECLARATIONS */
static void bl_flash_waitFsmReady(void);
static void bl_flash_waitFsmStatusSuccess(void);

/* PUBLIC FUNCTION DEFINITIONS */
bl_error_code_t bl_flash_FapiInitBank(uint32_t bankNum) {
  if ((Fapi_initializeFlashBanks(SYS_CLK_FREQ)) != Fapi_Status_Success) {
    return BL_ERR_CODE_UNKNOWN;
  }

  if (Fapi_setActiveFlashBank((Fapi_FlashBankType)bankNum) != Fapi_Status_Success) {
    return BL_ERR_CODE_INVALID_ARG;
  }

  if (Fapi_enableMainBankSectors(0xFF00U) != Fapi_Status_Success) {
    return BL_ERR_CODE_UNKNOWN;
  }

  // Possible infinite loop, but watchdog should reset the device if it gets stuck
  bl_flash_waitFsmReady();
  bl_flash_waitFsmStatusSuccess();

  return BL_ERR_CODE_SUCCESS;
}

bl_error_code_t bl_flash_FapiBlockErase(uint32_t startAddr, uint32_t size) {
  bl_error_code_t errCode = BL_ERR_CODE_SUCCESS;

  const uint32_t endAddr = startAddr + size;

  // Find the start and end of the sectors to erase. Assume flashSectors is sorted
  // by start address, and that the first sector starts at address 0

  uint8_t startSector = 0U;
  for (uint8_t i = 0U; i < NUM_FLASH_SECTORS; i++) {
    const uint32_t sectorStartAddr = (uint32_t)(flashSectors[i].start);
    if (sectorStartAddr > startAddr) {
      startSector = i - 1U;  // The previous sector contains the start address
      break;
    }
  }

  uint8_t endSector = 0U;
  for (uint8_t i = 0U; i < NUM_FLASH_SECTORS; i++) {
    const uint32_t sectorEndAddr = (uint32_t)(flashSectors[i].start) + flashSectors[i].length;
    if (sectorEndAddr >= endAddr) {
      endSector = i;
      break;
    }
  }

  for (uint8_t i = startSector; i < endSector + 1U; i++) {
    if (Fapi_issueAsyncCommandWithAddress(Fapi_EraseSector, flashSectors[i].start) != Fapi_Status_Success) {
      errCode = BL_ERR_CODE_UNKNOWN;
      break;
    }

    bl_flash_waitFsmReady();
    bl_flash_waitFsmStatusSuccess();
  }

  return errCode;
}

bl_error_code_t bl_flash_FapiBlockWrite(uint32_t dstAddr, uint32_t srcAddr, uint32_t numBytes) {
  bl_error_code_t errCode = BL_ERR_CODE_SUCCESS;

  register uint32_t src = srcAddr;
  register uint32_t dst = dstAddr;

  // Programming at an address is limited to the bank width number of bytes
  uint32_t bytesToFlashNext = numBytes < 16 ? numBytes : 16;

  while (numBytes > 0) {
    if (Fapi_issueProgrammingCommand((uint32_t *)dst, (uint8_t *)src, (uint32_t)bytesToFlashNext, NULL, 0,
                                     Fapi_DataOnly) != Fapi_Status_Success) {
      errCode = BL_ERR_CODE_UNKNOWN;
      break;
    }

    bl_flash_waitFsmReady();
    bl_flash_waitFsmStatusSuccess();

    src += bytesToFlashNext;
    dst += bytesToFlashNext;

    numBytes -= bytesToFlashNext;

    if (numBytes < 16) {
      bytesToFlashNext = numBytes;
    }
  }

  return errCode;
}

bool bl_flash_isStartAddrValid(uint32_t addr, uint32_t binSize) {
  const uint32_t lastFlashAddr =
      (uint32_t)flashSectors[NUM_FLASH_SECTORS - 1].start + flashSectors[NUM_FLASH_SECTORS - 1].length;

  if (addr + binSize > lastFlashAddr) {
    return false;
  }

  /* The start address must be at the beginning of the sector */
  bool isSectionStart = false;
  for (uint8_t i = 0; i < NUM_FLASH_SECTORS; i++) {
    if (addr == (uint32_t)(flashSectors[i].start)) {
      isSectionStart = true;
      break;
    }
  }

  if (!isSectionStart) {
    return false;
  }

  return true;
}

/* PRIVATE FUNCTION DEFINITIONS */
static void bl_flash_waitFsmReady(void) {
  while (FAPI_CHECK_FSM_READY_BUSY != Fapi_Status_FsmReady) {
    asm(" NOP");
  }
}

static void bl_flash_waitFsmStatusSuccess(void) {
  while (FAPI_GET_FSM_STATUS != Fapi_Status_Success) {
    asm(" NOP");
  }

  for (volatile uint32_t i = 0U; i < 100000UL; i++) {
    asm(" NOP");
  }
}
