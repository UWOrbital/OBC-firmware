#include "obc_spi_dma.h"
#include "spi.h"
#include "sys_dma.h"
#include "obc_errors.h"
#include "obc_logging.h"

#include <FreeRTOS.h>
#include <os_semphr.h>
#include <sys_common.h>
#include <FreeRTOSConfig.h>

#define DMA_SPI_FINISHED_SEMAPHORE_TIMEOUT pdMS_TO_TICKS(100000)
#define DMA_SPI_MUTEX_TIMEOUT pdMS_TO_TICKS(10000)
#define DMA_PORT_B 0x04

static SemaphoreHandle_t dmaSpi1FinishedSemaphore = NULL;
static StaticSemaphore_t dmaSpi1FinishedSemaphoreBuffer;
static SemaphoreHandle_t dmaSpi1Mutex = NULL;
static StaticSemaphore_t dmaSPi1MutexBuffer;

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
static void initDmaSpi1Semaphores(void);

/**
 * @brief initializes the semaphores for using the DMA for SPI1
 */
static void initDmaSpi1Semaphores(void) {
  if (dmaSpi1FinishedSemaphore == NULL) {
    dmaSpi1FinishedSemaphore = xSemaphoreCreateBinaryStatic(&dmaSpi1FinishedSemaphoreBuffer);
  }
  if (dmaSpi1Mutex == NULL) {
    dmaSpi1Mutex = xSemaphoreCreateMutex(&dmaSpi1Mutex);
  }
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
  spiReg->GCR1 |= (0x1 << 24);
  spiReg->INT0 |= (0x1 << 16);
  spiEnableNotification(spiReg, 0x10000);
  switch ((uint32_t)spiReg) {
    case (uint32_t)spiREG1:
      spiREG1->PC0 = 0 | (1 << 11)       // SOMI[0] as functional pin
                     | (1 << 10)         // SIMO[0] as functional pin
                     | (1 << 9)          // CLK as functional pin
                     | (0);              // CS as GIO pin
      spiREG1->PC1 = 0x01;               // CS[0] as output
      spiREG1->PC3 = 0x00;               // CS[0]=0
      dmaReqAssign(DMA_CH0, 0);          // Assign SPI1 RX to DMA channel 0
      dmaReqAssign(DMA_CH1, 1);          // Assign SPI1 TX to DMA channel 1
      dmaEnableInterrupt(DMA_CH0, BTC);  // Set DMA to trigger interrupt after a block transfer is complete
      initDmaSpi1Semaphores();           // Initialize dma spi1 semaphores
      xSemaphoreTake(dmaSpi1FinishedSemaphore, (TickType_t)0);  // initialize semaphore with value of 0
      dmaSetChEnable(DMA_CH0, DMA_HW);                          // SPI1 RX, hardware triggering
      dmaSetChEnable(DMA_CH1, DMA_HW);                          // SPI1 TX, hardware triggering
      break;
    // Add more cases as we start to implement different spi buses with DMA
    default:
      return OBC_ERR_CODE_INVALID_ARG;
  }
  spiReg->GCR1 = (spiREG1->GCR1 & 0xFFFFFFFFU) | (0x1 << 24);  // Enable SPI
  return OBC_ERR_CODE_SUCCESS;
}

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
static obc_error_code_t spiDmaConfig(spiBASE_t *spiReg, uint32_t txDataAddr, uint32_t rxDataAddr, size_t dataLen) {
  if (spiReg == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  if (txDataAddr == 0) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  if (rxDataAddr == 0) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  if (dataLen == 0) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  g_dmaCTRL dmaCtrlPktRx = {0};
  g_dmaCTRL dmaCtrlPktTx = {0};

  dmaCtrlPktRx.PORTASGN = DMA_PORT_B;
  dmaCtrlPktRx.SADD = (uint32)(&spiReg->BUF);
  dmaCtrlPktRx.DADD = (uint32)(rxDataAddr)-2;
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
  dmaCtrlPktTx.DADD = (uint32)(&spiReg->DAT1);
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

  dmaSetCtrlPacket(DMA_CH0, dmaCtrlPktRx);
  dmaSetCtrlPacket(DMA_CH1, dmaCtrlPktTx);

  return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief Uses the DMA to transmit and receive bytes over SPI
 *
 * @param spiReg the SPI bus to use for the transfer
 * @param spiDataFormat the spi data format to use for the transfer
 * @param txData array of uint16_t to be sent over the SPI bus
 * @param rxData buffer of atleast dataLen size to hold the received bytes
 * @param dataLen number of uint16_t to send
 *
 * @return obc_error_code_t - whether or not the transfer was successful
 */
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

  switch ((uint32_t)spiReg) {
    case (uint32_t)spiREG1:
      if (xSemaphoreTake(dmaSpi1Mutex, DMA_SPI_MUTEX_TIMEOUT) != pdPASS) {
        return OBC_ERR_CODE_MUTEX_TIMEOUT;
      }
      break;
    default:
      return OBC_ERR_CODE_INVALID_ARG;
  }

  RETURN_IF_ERROR_CODE(spiDmaConfig(spiReg, (uint32_t)txData, (uint32_t)rxData, dataLen));

  spiTransmitAndReceiveData(spiReg, spiDataFormat, dataLen, txData, rxData);

  switch ((uint32_t)spiReg) {
    case (uint32_t)spiREG1:
      if (xSemaphoreTake(dmaSpi1FinishedSemaphore, DMA_SPI_FINISHED_SEMAPHORE_TIMEOUT) != pdPASS) {
        return OBC_ERR_CODE_SEMAPHORE_TIMEOUT;
      }
      xSemaphoreGive(dmaSpi1Mutex);
      break;
    default:
      return OBC_ERR_CODE_INVALID_ARG;
  }

  return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief callback function to be called from ISR when DMA Block transfer is complete
 */
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
