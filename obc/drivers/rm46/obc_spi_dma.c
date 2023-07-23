#include "obc_spi_dma.h"
#include "spi.h"
#include "sys_dma.h"
#include "obc_errors.h"
#include "obc_logging.h"
#include "obc_privilege.h"
#include "obc_spi_io.h"
#include "obc_board_config.h"

#include <FreeRTOS.h>
#include <os_semphr.h>
#include <sys_common.h>
#include <FreeRTOSConfig.h>

#define DMA_SPI_FINISHED_SEMAPHORE_TIMEOUT pdMS_TO_TICKS(100000)
#define DMA_SPI_MUTEX_TIMEOUT portMAX_DELAY
#define DMA_PORT_B 0x04
#define SPI_NOTIFICATION_DMA_REQ 0x10000

static void dmaSpi1FinishedCallback(void);
static void dmaSpi3FinishedCallback(void);

static SemaphoreHandle_t dmaSpi1FinishedSemaphore = NULL;
static StaticSemaphore_t dmaSpi1FinishedSemaphoreBuffer;
static SemaphoreHandle_t dmaSpi3FinishedSemaphore = NULL;
static StaticSemaphore_t dmaSpi3FinishedSemaphoreBuffer;

/**
 * @brief configures DMA for a single spi transmission
 *
 * @param spiReg SPI bus to use for the transfer
 * @param txDataAddr starting address of the data that needs to be sent
 * @param rxDataAddr starting address of the buffer for received data
 * @param dataLen number of uint16_t to send
 *
 * @return obc_error_code_t - whether or not the configuration was successful
 */
static obc_error_code_t spiDmaConfig(spiBASE_t *spiReg, uint32_t txDataAddr, uint32_t rxDataAddr, size_t dataLen);

/**
 * @brief initializes the semaphores for using the DMA for SPI1
 */
void initDmaSpiSemaphores(void) {
  if (dmaSpi1FinishedSemaphore == NULL) {
    dmaSpi1FinishedSemaphore = xSemaphoreCreateBinaryStatic(&dmaSpi1FinishedSemaphoreBuffer);
  }
  if (dmaSpi3FinishedSemaphore == NULL) {
    dmaSpi3FinishedSemaphore = xSemaphoreCreateBinaryStatic(&dmaSpi3FinishedSemaphoreBuffer);
  }
  // Add more semaphores in the future as needed
}

/**
 * @brief initalizes DMA for facilitating spi transfers for spiReg
 *
 * @param SpiReg SPI port that the DMA should be enabled for
 *
 * @return obc_error_code_t - whether or not the initialization was successful
 */
obc_error_code_t spiDmaInit(spiBASE_t *spiReg) {
  if (spiReg == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  switch ((uint32_t)spiReg) {
    case (uint32_t)spiREG1:
      dmaReqAssign(DMA_SPI_1_RX_CHANNEL,
                   DMA_SPI1_RX_REQ_LINE);  // Assign SPI1 RX to DMA channel 0 (See Table 6-33 in datasheet)
      dmaReqAssign(DMA_SPI_1_TX_CHANNEL,
                   DMA_SPI1_TX_REQ_LINE);             // Assign SPI1 TX to DMA channel 1 (See Table 6-33 in datasheet)
      dmaEnableInterrupt(DMA_SPI_1_RX_CHANNEL, BTC);  // Set DMA to trigger interrupt after a block transfer is complete
      dmaSetChEnable(DMA_SPI_1_RX_CHANNEL, DMA_HW);   // SPI1 RX, hardware triggering
      dmaSetChEnable(DMA_SPI_1_TX_CHANNEL, DMA_HW);   // SPI1 TX, hardware triggering
      break;
    case (uint32_t)spiREG3:
      dmaReqAssign(DMA_SPI_3_RX_CHANNEL,
                   DMA_SPI3_RX_REQ_LINE);  // Assign SPI3 RX to DMA channel 2 (See Table 6-33 in datasheet)
      dmaReqAssign(DMA_SPI_3_TX_CHANNEL,
                   DMA_SPI3_TX_REQ_LINE);             // Assign SPI3 TX to DMA channel 3 (See Table 6-33 in datasheet)
      dmaEnableInterrupt(DMA_SPI_3_RX_CHANNEL, BTC);  // Set DMA to trigger interrupt after a block transfer is complete
      dmaSetChEnable(DMA_SPI_3_RX_CHANNEL, DMA_HW);   // SPI3 RX, hardware triggering
      dmaSetChEnable(DMA_SPI_3_TX_CHANNEL, DMA_HW);   // SPI3 TX, hardware triggering
      break;
    // Add more cases as we start to implement different spi buses with DMA
    default:
      return OBC_ERR_CODE_INVALID_ARG;
  }
  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t spiDmaConfig(spiBASE_t *spiReg, uint32_t txDataAddr, uint32_t rxDataAddr, size_t dataLen) {
  if (spiReg == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  if (dataLen == 0) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  g_dmaCTRL dmaCtrlPktRx = {0};
  g_dmaCTRL dmaCtrlPktTx = {0};

  dmaCtrlPktRx.PORTASGN = DMA_PORT_B;
  dmaCtrlPktRx.SADD = (uint32)(&spiReg->BUF);
  dmaCtrlPktRx.DADD = (uint32)(rxDataAddr);
  dmaCtrlPktRx.FRCNT = dataLen;
  dmaCtrlPktRx.ELCNT = 1;
  dmaCtrlPktRx.CHCTRL = 0;
  dmaCtrlPktRx.ELDOFFSET = 0;
  dmaCtrlPktRx.ELSOFFSET = 0;
  dmaCtrlPktRx.FRDOFFSET = 0;
  dmaCtrlPktRx.FRSOFFSET = 0;
  dmaCtrlPktRx.RDSIZE = ACCESS_16_BIT;
  dmaCtrlPktRx.WRSIZE = ACCESS_16_BIT;
  dmaCtrlPktRx.TTYPE = FRAME_TRANSFER;
  dmaCtrlPktRx.ADDMODERD = ADDR_FIXED;
  dmaCtrlPktRx.ADDMODEWR = ADDR_INC1;
  dmaCtrlPktRx.AUTOINIT = AUTOINIT_ON;

  dmaCtrlPktTx.PORTASGN = DMA_PORT_B;
  dmaCtrlPktTx.SADD = (uint32)(txDataAddr);
  dmaCtrlPktTx.DADD = (uint32)(&spiReg->DAT0);
  dmaCtrlPktTx.FRCNT = dataLen;
  dmaCtrlPktTx.ELCNT = 1;
  dmaCtrlPktTx.CHCTRL = 0;
  dmaCtrlPktTx.ELDOFFSET = 0;
  dmaCtrlPktTx.ELSOFFSET = 0;
  dmaCtrlPktTx.FRDOFFSET = 0;
  dmaCtrlPktTx.FRSOFFSET = 0;
  dmaCtrlPktTx.RDSIZE = ACCESS_16_BIT;
  dmaCtrlPktTx.WRSIZE = ACCESS_16_BIT;
  dmaCtrlPktTx.TTYPE = FRAME_TRANSFER;
  dmaCtrlPktTx.ADDMODERD = ADDR_INC1;
  dmaCtrlPktTx.ADDMODEWR = ADDR_FIXED;
  dmaCtrlPktTx.AUTOINIT = AUTOINIT_ON;

  switch ((uint32_t)spiReg) {
    case (uint32_t)spiREG1:
      dmaSetCtrlPacket(DMA_SPI_1_RX_CHANNEL, dmaCtrlPktRx);
      dmaSetCtrlPacket(DMA_SPI_1_TX_CHANNEL, dmaCtrlPktTx);
      break;
    case (uint32_t)spiREG3:
      dmaSetCtrlPacket(DMA_SPI_3_RX_CHANNEL, dmaCtrlPktRx);
      dmaSetCtrlPacket(DMA_SPI_3_TX_CHANNEL, dmaCtrlPktTx);
      break;
    default:
      return OBC_ERR_CODE_INVALID_ARG;
  }

  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t dmaSpiTransmitandReceiveBytes(spiBASE_t *spiReg, uint16_t *txData, uint16_t *rxData, size_t dataLen,
                                               uint32_t spiMutexTimeoutMs, uint32_t transferCompleteTimeoutMs) {
  if (spiReg != spiREG1 /* add more checks with `&&` as we start to support more spi buses with DMA */) {
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

  SemaphoreHandle_t spiMutex;
  RETURN_IF_ERROR_CODE(getSpiMutex(spiReg, &spiMutex));

  if (!isSpiBusOwner(spiMutex)) {
    return OBC_ERR_CODE_NOT_MUTEX_OWNER;
  }
  BaseType_t xRunningPriveleged = prvRaisePrivilege();

  RETURN_IF_ERROR_CODE(spiDmaConfig(spiReg, (uint32_t)txData, (uint32_t)rxData, dataLen));

  spiEnableNotification(spiReg, SPI_NOTIFICATION_DMA_REQ);

  switch ((uint32_t)spiReg) {
    case (uint32_t)spiREG1:
      if (xSemaphoreTake(dmaSpi1FinishedSemaphore, pdMS_TO_TICKS(transferCompleteTimeoutMs)) != pdPASS) {
        return OBC_ERR_CODE_SEMAPHORE_TIMEOUT;
      }
      break;
    case (uint32_t)spiREG3:
      if (xSemaphoreTake(dmaSpi3FinishedSemaphore, pdMS_TO_TICKS(transferCompleteTimeoutMs)) != pdPASS) {
        return OBC_ERR_CODE_SEMAPHORE_TIMEOUT;
      }
    default:
      return OBC_ERR_CODE_INVALID_ARG;
  }
  portRESET_PRIVILEGE(xRunningPriveleged);

  return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief callback function to be called from ISR when DMA Block transfer is complete
 */
static void dmaSpi1FinishedCallback(void) {
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

/**
 * @brief callback function to be called from ISR when DMA Block transfer is complete
 */
static void dmaSpi3FinishedCallback(void) {
  BaseType_t xHigherPriorityTaskAwoken = pdFALSE;
  // give semaphore and set xHigherPriorityTaskAwoken to pdTRUE if this unblocks a higher priority task than the current
  // one
  if (xSemaphoreGiveFromISR(dmaSpi3FinishedSemaphore, &xHigherPriorityTaskAwoken) != pdPASS) {
    /* TODO: figure out how to log from ISR */
  }
  // if xHigherPriorityTaskAwoken == pdTRUE then request a context switch since this means a higher priority task has
  // been unblocked
  portYIELD_FROM_ISR(xHigherPriorityTaskAwoken);
}

void dmaGroupANotification(dmaInterrupt_t inttype, uint32 channel) {
  /*  enter user code between the USER CODE BEGIN and USER CODE END. */
  /* USER CODE BEGIN (54) */
  switch (inttype) {
    case FTC:
    case LFS:
    case BTC:
      switch (channel) {
        case DMA_SPI_1_RX_CHANNEL:
          dmaSpi1FinishedCallback();
          break;
        case DMA_SPI_3_RX_CHANNEL:
          dmaSpi3FinishedCallback();
          break;
      }
    case HBC:
  }
  /* USER CODE END */
}
