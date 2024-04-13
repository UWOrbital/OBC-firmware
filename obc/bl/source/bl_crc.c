#include "bl_crc.h"
#include "bl_errors.h"

#include "crc.h"
#include "reg_crc.h"
#include "sys_dma.h"

#include <stdint.h>

#define CRC_MAX_TIMEOUT 0x00U  // Setting it to 0, disables the timeout.
#define CRC_SECTOR_COUNT 0x01U
#define CRC_CHANNEL CRC_CH1
#define CRC_MODE CRC_SEMI_CPU
#define CRC_BASE crcREG

// DMA associated signals
#define CRC_DMA_PORT 0x04  // Only PORT B seems to be valid
#define CRC_DMA_FRAME_COUNT 0x01U
#define CRC_DMA_CHANNEL DMA_CH26
#define CRC_DMA_REQLINE 26U

static const uint32_t PSA_SIGNATURE_REGISTER = CRC_BASE->PSA_SIGREGH1;

static enum { CRC_FLAG_CLEAR, CRC_FLAG_SET } crcCompleteFlag = CRC_FLAG_CLEAR;
static enum { CRC_TIMED_OUT, CRC_NO_TIMEOUT } crcTimeoutFlag = CRC_NO_TIMEOUT;
static crcConfig_t crcConfig = {0};

static bl_error_code_t requestCrcDmaTransfer(uint32_t startAddress, uint32_t patternCount);

bl_error_code_t blCrcInit() {
  crcInit();
  crcEnableNotification(CRC_BASE, CRC_CH1_CC | CRC_CH1_TO);

  crcCompleteFlag = CRC_FLAG_CLEAR;
  crcTimeoutFlag = CRC_NO_TIMEOUT;

  crcConfig.mode = CRC_MODE;
  crcConfig.crc_channel = CRC_CHANNEL;
  crcConfig.scount = CRC_SECTOR_COUNT;
  crcConfig.wdg_preload = CRC_MAX_TIMEOUT;
}

bl_error_code_t performCrcVerification(crc_dma_request_t* request) {
  crcConfig.pcount = request->crcElementCount;
  crcConfig.block_preload = request->crcSeedValue;

  crcSetConfig(CRC_BASE, &crcConfig);
  RETURN_IF_ERROR_CODE(requestCrcDmaTransfer((uint32_t)request->crcStartAddress, request->crcElementCount));

  while (crcCompleteFlag != CRC_FLAG_SET || crcTimeoutFlag != CRC_TIMED_OUT) continue;
  if (crcTimeoutFlag == CRC_TIMED_OUT) {
    return BL_ERR_CODE_CRC_TIMEOUT;
  }

  uint64_t crcResult = crcGetSectorSig();
  if (crcResult != request->crcExpectedValue) {
    return BL_ERR_CODE_CRC_FAILURE;
  }

  crcClearCompleteFlag = CRC_FLAG_CLEAR;
  crcTimeoutFlag = CRC_NO_TIMEOUT;
  return BL_ERR_CODE_SUCCESS;
}

void crcNotification(crcBASE_t* crc, uint32 flags) {
  if (flags & CRC_CH1_CC) {
    crcCompleteFlag = CRC_FLAG_SET;
    return;
  }
  crcTimeoutFlag = CRC_TIMED_OUT;
}

static bl_error_code_t requestCrcDmaTransfer(uint32_t startAddress, uint32_t patternCount) {
  g_dmaCTRL dmaCtrlPkt = {0};
  dmaCtrlPkt.SADD = startAddress;
  dmaCtrlPkt.DADD = CRC_PSA_REGISTER;
  dmaCtrlPkt.FRCNT = CRC_DMA_FRAME_COUNT;
  dmaCtrlPkt.ELCNT = patternCount;
  dmaCtrlPkt.ADDMODERD = ADDR_INC1;
  dmaCtrlPkt.ADDMODEWR = ADDR_FIXED;
  dmaCtrlPkt.PORTASGN = CRC_DMA_PORT;
  dmaCtrlPkt.RDSIZE = ACCESS_8_BIT;
  dmaCtrlPkt.WRSIZE = ACCESS_64_BIT;
  dmaCtrlPkt.TTYPE = FRAME_TRANSFER;

  dmaSetCtrlPacket(CRC_DMA_CHANNEL, dmaCtrlPkt);
  dmaReqAssign(CRC_DMA_CHANNEL, CRC_DMA_REQLINE);
  dmaSetChEnable(CRC_CHANNEL, DMA_SW);
  return BL_ERR_CODE_SUCCESS;
}
