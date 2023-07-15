#include "obc_mibspi_dma.h"
#include "sys_dma.h"

// /**
//  * @brief Take the mutex for the specified SPI port and chip select pin.
//  *
//  * @param spiReg The SPI register to use.
//  *
//  * @note This will not assert the CS pin. This function is intended to be used
//  * to send data to a device that requires the CS pin to be deasserted.
//  *
//  * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if successful, error code otherwise.
//  */
// obc_error_code_t spiTakeBusMutex(spiBASE_t *spiReg);

// /**
//  * @brief Release the mutex for the specified SPI port and chip select pin.
//  *
//  * @param spiReg The SPI register to use.
//  *
//  * @note This will not deassert the CS pin. This function is intended to be used
//  * to send data to a device that requires the CS pin to be deasserted.
//  *
//  * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if successful, error code otherwise.
//  */
// obc_error_code_t dmaSpiReleaseBusMutex(spiBASE_t *spiReg);

// /**
//  * @brief Send a byte via SPI.
//  * @param spiReg The SPI register to use.
//  * @param spiDataFormat The SPI data format options.
//  * @param outb The byte to send.
//  * @return Error code. OBC_ERR_CODE_SUCCESS if successful.
//  */
// obc_error_code_t dmaSpiTransmitByte(spiBASE_t *spiReg, spiDAT1_t *spiDataFormat, uint8_t outb){

// }

// /**
//  * @brief Send multiple bytes via SPI.
//  * @param spiReg The SPI register to use.
//  * @param spiDataFormat The SPI data format options.
//  * @param outBytes The bytes to send.
//  * @param numBytes The number of bytes to send.
//  * @return Error code. OBC_ERR_CODE_SUCCESS if successful.
//  */
// obc_error_code_t dmaSpiTransmitBytes(spiBASE_t *spiReg, spiDAT1_t *spiDataFormat, uint8_t *outBytes, size_t
// numBytes);

// /**
//  * @brief Receive a byte via SPI.
//  * @param spiReg The SPI register to use.
//  * @param spiDataFormat The SPI data format options.
//  * @param inb Buffer to store the received byte.
//  * @return Error code. OBC_ERR_CODE_SUCCESS if successful.
//  */
// obc_error_code_t dmaSpiReceiveByte(spiBASE_t *spiReg, spiDAT1_t *spiDataFormat, uint8_t *inb);

// /**
//  * @brief Receive multiple bytes via SPI.
//  * @param spiReg The SPI register to use.
//  * @param spiDataFormat The SPI data format options.
//  * @param inBytes Buffer to store the received byte.
//  * @param numBytes The number of bytes to receive
//  * @return Error code. OBC_ERR_CODE_SUCCESS if successful.
//  */
// obc_error_code_t dmaSpiReceiveBytes(spiBASE_t *spiReg, spiDAT1_t *spiDataFormat, uint8_t *inBytes, size_t numBytes);

// /**
//  * @brief Send and receive a byte via SPI.
//  * @param spiReg The SPI register to use.
//  * @param spiDataFormat The SPI data format options.
//  * @param outb The byte to send.
//  * @param inb Buffer to store the received byte.
//  * @return Error code. OBC_ERR_CODE_SUCCESS if successful.
//  */
// obc_error_code_t dmaSpiTransmitAndReceiveByte(spiBASE_t *spiReg, spiDAT1_t *spiDataFormat, uint8_t outb, uint8_t
// *inb);

// /**
//  * @brief Send and receive multiple bytes via SPI.
//  * @param spiReg The SPI register to use.
//  * @param spiDataFormat The SPI data format options.
//  * @param outBytes The byte to send.
//  * @param inBytes Buffer to store the received byte.
//  * @param numBytes The number of bytes to send and receive
//  * @return Error code. OBC_ERR_CODE_SUCCESS if successful.
//  */
// obc_error_code_t dmaSpiTransmitAndReceiveBytes(spiBASE_t *spiReg, spiDAT1_t *spiDataFormat, uint8_t *outBytes,
//                                                uint8_t *inBytes, size_t numBytes);

void mibspiDmaConfig(mibspiBASE_t *mibspi, uint32 channel, uint32 txchannel, uint32 rxchannel, uint32_t dataLen,
                     uint16_t *txData) {
  /* - assigning dma request: channel-0 with request line - 1 */
  dmaReqAssign(0, 1);

  g_dmaCTRL dmaCtrl;
  /* - configuring dma control packets   */
  /* upto 32 control packets are supported. */
  dmaCtrl.SADD = (uint32)(&txData);                   /* source address             */
  dmaCtrl.DADD = (uint32)(&(mibspiRAM1->tx[0].data)); /* destination  address       */
  dmaCtrl.CHCTRL = 0;                                 /* channel control            */
  dmaCtrl.FRCNT = 1;                                  /* frame count                */
  dmaCtrl.ELCNT = dataLen;                            /* element count              */
  dmaCtrl.ELDOFFSET = 4;                              /* element destination offset */
  dmaCtrl.ELSOFFSET = 0;                              /* element destination offset */
  dmaCtrl.FRDOFFSET = 0;                              /* frame destination offset   */
  dmaCtrl.FRSOFFSET = 0;                              /* frame destination offset   */
  dmaCtrl.PORTASGN = 4;                               /* port b                     */
  dmaCtrl.RDSIZE = ACCESS_16_BIT;                     /* read size                  */
  dmaCtrl.WRSIZE = ACCESS_16_BIT;                     /* write size                 */
  dmaCtrl.TTYPE = FRAME_TRANSFER;                     /* transfer type              */
  dmaCtrl.ADDMODERD = ADDR_INC1;                      /* address mode read          */
  dmaCtrl.ADDMODEWR = ADDR_OFFSET;                    /* address mode write         */
  dmaCtrl.AUTOINIT = AUTOINIT_ON;                     /* autoinit                   */

  /* - setting dma control packets */
  dmaSetCtrlPacket(DMA_CH0, dmaCtrl);

  /* - setting the dma channel to trigger on h/w request */
  dmaSetChEnable(DMA_CH0, DMA_HW);

  uint32 buf = 0;
  uint32 initialCount = 0;

  /* setting transmit and receive channels */
  mibspi->DMACTRL[channel] |= (((rxchannel << 4) | txchannel) << 16);

  /* enabling transmit and receive dma */
  mibspi->DMACTRL[channel] |= 0x8000C000;

  /* setting Initial Count of DMA transfers and the buffer utilized for DMA transfer */
  mibspi->DMACTRL[channel] |= (initialCount << 8) | (buf << 24);
}

void mibspiDmaInit(void) {}
