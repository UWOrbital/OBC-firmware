#include "F021.h"
#include "flash_config.h"
#include "bl_config.h"
#include "bl_flash.h"

#include <stdint.h>
#include <stdbool.h>

static bool bankInitialized = false;

uint32_t Fapi_Init(uint32_t startBank, uint32_t endBank) {
  if ((Fapi_initializeFlashBanks(SYS_CLK_FREQ)) == Fapi_Status_Success) {
    for (uint32_t i = startBank; i < endBank + 1; i++) {
      (void)Fapi_setActiveFlashBank((Fapi_FlashBankType)i);
      (void)Fapi_enableMainBankSectors(0xFFFF);
      while (FAPI_CHECK_FSM_READY_BUSY != Fapi_Status_FsmReady)
        ;
      while (FAPI_GET_FSM_STATUS != Fapi_Status_Success)
        ;
    }

    bankInitialized = true;
    return 0;
  }

  return 1;
}

uint32_t Fapi_BlockErase(uint32_t startAddr, uint32_t size) {
  uint8_t startSector = 0;
  uint8_t endSector = 0;

  uint32_t endAddr = startAddr + size;

  uint8_t startBankNum = 0;
  for (int i = 0; i < NUM_FLASH_SECTORS; i++) {
    if (startAddr < (uint32_t)(flashSectors[i].start)) {
      startBankNum = flashSectors[i - 1].bankNumber;
      startSector = i - 1;
      break;
    }
  }

  uint8_t endBankNum = 0;
  for (int i = startSector; i < NUM_FLASH_SECTORS; i++) {
    if (endAddr <= (((uint32_t)flashSectors[i].start) + flashSectors[i].length)) {
      endBankNum = flashSectors[i].bankNumber;
      endSector = i;
      break;
    }
  }

  uint32_t status;  // TODO: Add error codes

  status = Fapi_Init(startBankNum, endBankNum);

  for (int i = startSector; i < endSector + 1; i++) {
    Fapi_issueAsyncCommandWithAddress(Fapi_EraseSector, flashSectors[i].start);
    while (FAPI_CHECK_FSM_READY_BUSY == Fapi_Status_FsmBusy)
      ;
    while (FAPI_GET_FSM_STATUS != Fapi_Status_Success)
      ;
  }

  return status;
}

uint32_t Fapi_BlockProgram(uint32_t flashAddress, uint32_t dataAddress, uint32_t numBytes) {
  register uint32_t src = dataAddress;
  register uint32_t dst = flashAddress;

  uint32_t bytes = numBytes < 16 ? numBytes : 16;

  if (!bankInitialized) {
    return 1;
  }

  while (numBytes > 0) {
    Fapi_issueProgrammingCommand((uint32_t *)dst, (uint8_t *)src, (uint32_t)bytes, 0, 0, Fapi_AutoEccGeneration);

    while (FAPI_CHECK_FSM_READY_BUSY == Fapi_Status_FsmBusy)
      ;
    while (FAPI_GET_FSM_STATUS != Fapi_Status_Success)
      ;

    src += bytes;
    dst += bytes;
    numBytes -= bytes;

    if (numBytes < 32) {
      bytes = numBytes;
    }
  }

  return 0;
}
