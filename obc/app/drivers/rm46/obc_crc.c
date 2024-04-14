#include "obc_crc.h"
#include "obc_errors.h"

#include "crc.h"
#include "reg_crc.h"
#include "sys_dma.h"

#include "FreeRTOS.h"
#include "os_semphr.h"

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

#define CRC_QUEUE_LENGTH 1U

typedef enum { CRC_COMPLETE, CRC_FAILURE } crc_result_t;

static SemaphoreHandle_t crcMutex = NULL;
static StaticSemaphore_t crcMutexBuffer = {0};
static QueueHandle_t crcCompleteQueue = NULL;

static crcConfig_t crcConfig = {0};
static obc_error_code_t requestCrcDmaTransfer(uint32_t startAddress, uint32_t patternCount);

obc_error_code_t obcCrcInit() {
  crcInit();
  crcEnableNotification(CRC_BASE, CRC_CH1_CC | CRC_CH1_TO);

  crcMutex = xSemaphoreCreateMutexStatic(&crcMutexBuffer);
  ASSERT(crcMutex != NULL);

  crcCompleteQueue = xQueueCreate(CRC_QUEUE_LENGTH, sizeof(crc_result_t));
  ASSERT(crcCompleteQueue != NULL);

  crcConfig.mode = CRC_MODE;
  crcConfig.crc_channel = CRC_CHANNEL;
  crcConfig.scount = CRC_SECTOR_COUNT;
  crcConfig.wdg_preload = CRC_MAX_TIMEOUT;
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t performCrcVerification(crc_dma_request_t* request, TickType_t mutexTimeout, TickType_t dmaTimeout) {
  if (xSemaphoreTake(crcMutex, mutexTimeout) != pdPASS) {
    return OBC_ERR_CODE_MUTEX_TIMEOUT;
  }

  crcConfig.pcount = request->crcElementCount;
  crcConfig.block_preload = request->crcSeedValue;
  crcSetConfig(CRC_BASE, &crcConfig);
  RETURN_IF_ERROR_CODE(requestCrcDmaTransfer((uint32_t)request->crcStartAddress, request->crcElementCount));

  crc_result_t result = 0;
  obc_error_code_t errCode = OBC_ERR_CODE_SUCCESS;
  if (xQueueReceive(crcCompleteQueue, &result, dmaTimeout) != pdPASS) {
    errCode = OBC_ERR_CODE_CRC_DMA_TIMEOUT;
  }

  if (result != CRC_COMPLETE) {
    errCode = OBC_ERR_CODE_CRC_INTERRUPT_FAILURE;
  }

  uint64_t crcResult = crcGetSectorSig(CRC_BASE, CRC_CHANNEL);
  if (crcResult != request->crcExpectedValue) {
    errCode = OBC_ERR_CODE_CRC_FAILURE;
  }

  xSemaphoreGive(crcMutex);
  return errCode;
}

void crcNotification(crcBASE_t* crc, uint32 flags) {
  BaseType_t higherPriorityTaskWoken = pdFALSE;

  crc_result_t crcResult = CRC_FAILURE;

  if (flags & CRC_CH1_CC) {
    result = CRC_COMPLETE;
  }

  xQueueSendToBackFromISR(crcCompleteQueue, &crcResult, &higherPriorityTaskWoken);
  portYIELD_FROM_ISR(higherPriorityTaskWoken);
}

static obc_error_code_t requestCrcDmaTransfer(uint32_t startAddress, uint32_t patternCount) {
  g_dmaCTRL dmaCtrlPkt = {0};
  dmaCtrlPkt.SADD = startAddress;
  dmaCtrlPkt.DADD = (uint32_t)(&CRC_BASE->PSA_SIGREGH1);
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
  return OBC_ERR_CODE_SUCCESS;
}
