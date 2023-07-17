#include "obc_spi_dma.h"
#include "spi.h"
#include "sys_dma.h"
#include "obc_errors.h"
#include "obc_logging.h"

#include <FreeRTOS.h>
#include <os_semphr.h>
#include <sys_common.h>
#include <FreeRTOSConfig.h>

#define DMA_SPI_FINISHED_SEMAPHORE_TIMEOUT pdMS_TO_TICKS(10000)
#define DMA_PORT_B 0x04
#if ((__little_endian__ == 1) || (__LITTLE_ENDIAN__ == 1))
#define SPI1_TX_ADDR ((uint32_t)(&(spiREG1->DAT1)) + 0)
#define SPI1_RX_ADDR ((uint32_t)(&(spiREG1->BUF)) + 0)
#else
#define SPI1_TX_ADDR ((uint32_t)(&(spiREG1->DAT1)) + 2)
#define SPI1_RX_ADDR ((uint32_t)(&(spiREG1->BUF)) + 2)
#endif

static SemaphoreHandle_t dmaSpi1FinishedSemaphore = NULL;
static StaticSemaphore_t dmaSpi1FinishedSemaphoreBuffer;

static obc_error_code_t spiDmaConfig(spiBASE_t *spiReg, uint16_t *txData, uint16_t *rxData, size_t dataLen);
static void initDmaSpi1FinishedSemaphore(void);

static void initDmaSpi1FinishedSemaphore(void) {
  if (dmaSpi1FinishedSemaphore == NULL) {
    dmaSpi1FinishedSemaphore = xSemaphoreCreateBinaryStatic(&dmaSpi1FinishedSemaphoreBuffer);
  }
}

obc_error_code_t spiDmaInit(spiBASE_t *spiReg) {
  if (spiReg == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  spiReg->GCR1 |= (0x1 << 24);
  spiReg->INT0 |= (0x1 << 16);

  switch ((uint32_t)spiReg) {
    case (uint32_t)spiREG1:
      dmaReqAssign(DMA_CH0, 0);
      dmaReqAssign(DMA_CH1, 1);
      dmaEnableInterrupt(DMA_CH0, FTC);
      initDmaSpi1FinishedSemaphore();
      break;
    // Add more cases as start to implement different spi buses with
    default:
      return OBC_ERR_CODE_INVALID_ARG;
  }
  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t spiDmaConfig(spiBASE_t *spiReg, uint16_t *txData, uint16_t *rxData, size_t dataLen) {
  if (spiReg == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  if (txData == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  if (rxData == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  if (dataLen == 0) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  g_dmaCTRL dmaCtrlPktRx = {0};
  g_dmaCTRL dmaCtrlPktTx = {0};

  dmaCtrlPktRx.PORTASGN = DMA_PORT_B;
  dmaCtrlPktRx.SADD = (uint32)(&spiReg->BUF);
  dmaCtrlPktRx.DADD = (uint32)(rxData);
  dmaCtrlPktRx.FRCNT = 1;
  dmaCtrlPktRx.ELCNT = dataLen;
  dmaCtrlPktRx.CHCTRL = 0;
  dmaCtrlPktRx.ELDOFFSET = 0;
  dmaCtrlPktRx.ELSOFFSET = 0;
  dmaCtrlPktRx.FRDOFFSET = 0;
  dmaCtrlPktRx.FRSOFFSET = 0;
  dmaCtrlPktRx.PORTASGN = 4;
  dmaCtrlPktRx.RDSIZE = ACCESS_16_BIT;
  dmaCtrlPktRx.WRSIZE = ACCESS_16_BIT;
  dmaCtrlPktRx.TTYPE = FRAME_TRANSFER;
  dmaCtrlPktRx.ADDMODERD = ADDR_FIXED;
  dmaCtrlPktRx.ADDMODEWR = ADDR_INC1;
  dmaCtrlPktRx.AUTOINIT = AUTOINIT_ON;

  dmaCtrlPktTx.PORTASGN = DMA_PORT_B;
  dmaCtrlPktTx.SADD = (uint32)(txData);
  dmaCtrlPktTx.DADD = (uint32)(&spiReg->DAT0);
  dmaCtrlPktTx.FRCNT = 1;
  dmaCtrlPktTx.ELCNT = dataLen;
  dmaCtrlPktTx.CHCTRL = 0;
  dmaCtrlPktTx.ELDOFFSET = 0;
  dmaCtrlPktTx.ELSOFFSET = 0;
  dmaCtrlPktTx.FRDOFFSET = 0;
  dmaCtrlPktTx.FRSOFFSET = 0;
  dmaCtrlPktTx.PORTASGN = 4;
  dmaCtrlPktTx.RDSIZE = ACCESS_16_BIT;
  dmaCtrlPktTx.WRSIZE = ACCESS_16_BIT;
  dmaCtrlPktTx.TTYPE = FRAME_TRANSFER;
  dmaCtrlPktTx.ADDMODERD = ADDR_INC1;
  dmaCtrlPktTx.ADDMODEWR = ADDR_FIXED;
  dmaCtrlPktTx.AUTOINIT = AUTOINIT_ON;

  dmaSetCtrlPacket(DMA_CH0, dmaCtrlPktRx);
  dmaSetCtrlPacket(DMA_CH1, dmaCtrlPktTx);

  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t dmaSpiTransmitandReceiveBytes(spiBASE_t *spiReg, spiDAT1_t *spiDataFormat, uint16_t *txData,
                                               uint16_t *rxData, size_t dataLen) {
  if (spiReg == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  if (spiDataFormat == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  if (txData == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  if (rxData == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  if (dataLen == 0) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  obc_error_code_t errCode;

  RETURN_IF_ERROR_CODE(spiDmaConfig(spiReg, txData, rxData, dataLen));

  spiSendAndGetData(spiReg, spiDataFormat, dataLen, txData, rxData);

  switch ((uint32_t)spiReg) {
    case (uint32_t)spiREG1:
      if (xSemaphoreTake(dmaSpi1FinishedSemaphore, DMA_SPI_FINISHED_SEMAPHORE_TIMEOUT) != pdPASS) {
        return OBC_ERR_CODE_SEMAPHORE_TIMEOUT;
      }
      break;
    default:
      return OBC_ERR_CODE_INVALID_ARG;
  }

  return OBC_ERR_CODE_SUCCESS;
}

void dmaSpi1FinishedCallback(void) {
  BaseType_t xHigherPriorityTaskAwoken = pdFALSE;
  // give semaphore and set xHigherPriorityTaskAwoken to pdTRUE if this unblocks a higher priority task than the current
  // one
  if (xSemaphoreGiveFromISR(dmaSpi1FinishedSemaphore, &xHigherPriorityTaskAwoken) != pdPASS) {
    /* TODO: figure out how to log from ISR */
  }
  // if xHigherPriorityTaskAwoken == pdTRUE then request a context switch since this means a higher priority task has
  // been unblocked
  portYIELD_FROM_ISR(xHigherPriorityTaskAwoken);
}
