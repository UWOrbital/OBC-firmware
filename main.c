#include "obc_logging.h"
#include "supervisor.h"
#include "obc_sci_io.h"
#include "obc_i2c_io.h"
#include "obc_spi_io.h"
#include "sys_dma.h"
#include "obc_mibspi_dma.h"
#include "mibspi.h"
#include "obc_spi_dma.h"

#include <FreeRTOS.h>
#include <os_task.h>

#include <sys_common.h>
#include <sys_core.h>
#include <gio.h>
#include <sci.h>
#include <i2c.h>
#include <spi.h>
#include <can.h>
#include <het.h>
#include <system.h>
#include <stdint.h>

/* example data Pattern configuration */
#define D_SIZE 127

void loadDataPattern(uint32 psize, uint16* pptr);
void mibspiEnableInternalLoopback(mibspiBASE_t* mibspi);

uint16_t TX_DATA[D_SIZE];       /* transmit buffer in sys ram */
uint16_t RX_DATA[D_SIZE] = {0}; /* receive  buffer in sys ram */

static const spiDAT1_t spiConfig = {.CS_HOLD = false, .WDEL = false, .DFSEL = 1};

// g_dmaCTRL g_dmaCTRLPKT; /* dma control packet configuration stack */
/* USER CODE END */

/** @fn void main(void)
 *   @brief Application main function
 *
 */

/* USER CODE BEGIN (2) */
/* USER CODE END */

void main(void) {
  /* USER CODE BEGIN (3) */

  // /* - creating a data chunk in system ram to start with ... */
  // loadDataPattern(D_SIZE, &TX_DATA[0]);

  // /* - initializing mibspi - enabling tg 0 , length 127 (halcogen file)*/
  // mibspiInit();

  // /* - enabling loopback ( this is to emulate data transfer without external wires */
  // mibspiEnableInternalLoopback(mibspiREG1);

  // /* - configuring the mibspi dma , channel 0 , tx line -0 , rxline -1     */
  // /* - refer to the device data sheet dma request source for mibspi tx/rx  */
  // mibspiDmaConfig(mibspiREG1, 0, 0, 1, D_SIZE, TX_DATA);

  // /* - enabling dma module */
  // dmaEnable();

  // /* - start the mibspi transfer tg 0 */
  // mibspiTransfer(mibspiREG1, 0);

  // /* ... wait until transfer complete  */
  // while (!(mibspiIsTransferComplete(mibspiREG1, 0))) {
  // };

  // /* copy from mibspi ram to sys ram */
  // mibspiGetData(mibspiREG1, 0, RX_DATA);

  spiInit();

  dmaEnable();

  spiDmaInit(spiREG1);

  _enable_interrupt_();

  for (uint8_t i = 0; i < D_SIZE; ++i) {
    TX_DATA[i] = i;
  }

  spiEnableLoopback(spiREG1, 0);

  dmaSpiTransmitandReceiveBytes(spiREG1, &spiConfig, TX_DATA, RX_DATA, D_SIZE);

  while (1)
    ; /* loop forever */

  /* USER CODE END */
}

/* USER CODE BEGIN (4) */
/** void mibspiEnableLoopback(mibspiBASE_t *mibspi )
 *
 *   enabling internal loopback on mibspix
 */
void mibspiEnableInternalLoopback(mibspiBASE_t* mibspi) {
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
void loadDataPattern(uint32 psize, uint16* pptr) {
  *pptr = 0xD0C0;
  while (psize--) {
    *pptr = 0x1111 + *pptr++;
  }
}
/* USER CODE END */
