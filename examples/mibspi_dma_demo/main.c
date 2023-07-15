
/* USER CODE BEGIN (1) */
#include "obc_mibspi_dma.h"

#include <sys_common.h>
#include <system.h>
#include <mibspi.h>
#include <sys_dma.h>
#include <stdint.h>

/* example data Pattern configuration */
#define D_SIZE 127

void loadDataPattern(uint32 psize, uint16 *pptr);
void mibspiEnableInternalLoopback(mibspiBASE_t *mibspi);
void dmaConfigCtrlPacket(uint32 sadd, uint32 dadd, uint32 dsize);
void mibspiDmaConfig(mibspiBASE_t *mibspi, uint32 channel, uint32 txchannel, uint32 rxchannel);

uint16 TX_DATA[D_SIZE];       /* transmit buffer in sys ram */
uint16 RX_DATA[D_SIZE] = {0}; /* receive  buffer in sys ram */

g_dmaCTRL g_dmaCTRLPKT; /* dma control packet configuration stack */
/* USER CODE END */

/** @fn void main(void)
 *   @brief Application main function
 *
 */

/* USER CODE BEGIN (2) */
/* USER CODE END */

void main(void) {
  /* USER CODE BEGIN (3) */

  /* - creating a data chunk in system ram to start with ... */
  loadDataPattern(D_SIZE, &TX_DATA[0]);

  /* - initializing mibspi - enabling tg 0 , length 127 (halcogen file)*/
  mibspiInit();

  /* - enabling loopback ( this is to emulate data transfer without external wires */
  mibspiEnableInternalLoopback(mibspiREG1);

  /* - configuring the mibspi dma , channel 0 , tx line -0 , rxline -1     */
  /* - refer to the device data sheet dma request source for mibspi tx/rx  */
  mibspiDmaConfig(mibspiREG1, 0, 0, 1);

  /* - enabling dma module */
  dmaEnable();

  /* - start the mibspi transfer tg 0 */
  mibspiTransfer(mibspiREG1, 0);

  /* ... wait until transfer complete  */
  while (!(mibspiIsTransferComplete(mibspiREG1, 0))) {
  };

  /* copy from mibspi ram to sys ram */
  mibspiGetData(mibspiREG1, 0, RX_DATA);

  while (1)
    ; /* loop forever */

  /* USER CODE END */
}

/* USER CODE BEGIN (4) */
/** void mibspiEnableLoopback(mibspiBASE_t *mibspi )
 *
 *   enabling internal loopback on mibspix
 */
void mibspiEnableInternalLoopback(mibspiBASE_t *mibspi) {
  /* enabling internal loopback */
  mibspi->GCR1 |= 1U << 16U;
}

/** void loadDataPattern(uint32 psize, uint16* pptr)
 *
 *   loading a randam data chunk into system ram
 *
 *     pptr  > sys ram address
 *     psize > chunkl size
 *
 */
void loadDataPattern(uint32 psize, uint16 *pptr) {
  *pptr = 0xD0C0;
  while (psize--) {
    *pptr = 0x1111 + *pptr++;
  }
}
/* USER CODE END */
