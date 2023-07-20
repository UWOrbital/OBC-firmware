#include "obc_logging.h"
#include "supervisor.h"
#include "obc_sci_io.h"
#include "obc_i2c_io.h"
#include "obc_spi_io.h"
#include "sys_dma.h"
#include "mibspi.h"
#include "obc_spi_dma.h"

#include <FreeRTOS.h>
#include <os_task.h>

#include <stdio.h>
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

void task(void *pvParameters);

static StackType_t stack[1024];
static StaticTask_t taskBuf;
uint16_t TX_DATA[D_SIZE];       /* transmit buffer in sys ram */
uint16_t RX_DATA[D_SIZE] = {5}; /* receive  buffer in sys ram */

static const spiDAT1_t spiConfig = {.CS_HOLD = false, .WDEL = false, .DFSEL = 1};

/* USER CODE END */

/** @fn void main(void)
 *   @brief Application main function
 *
 */

/* USER CODE BEGIN (2) */
/* USER CODE END */

void main(void) {
  /* USER CODE BEGIN (3) */
  spiInit();
  sciInit();

  dmaEnable();

  initSciMutex();

  _enable_interrupt_();

  for (uint8_t i = 0; i < D_SIZE; ++i) {
    TX_DATA[i] = i;
  }

  spiEnableLoopback(spiREG1, 0);

  TaskHandle_t t = xTaskCreateStatic(task, "name", 1024U, NULL, 500, stack, &taskBuf);

  vTaskStartScheduler();

  while (1)
    ;
}

void task(void *pvParameters) {
  spiDmaInit(spiREG1);

  dmaSpiTransmitandReceiveBytes(spiREG1, &spiConfig, TX_DATA, RX_DATA, D_SIZE);

  for (volatile uint32_t i = 0; i < 100000; ++i)
    ;

  char str[10] = {'\0'};

  for (uint8_t i = 0; i < D_SIZE; ++i) {
    sprintf(str, "%u ", RX_DATA[i]);
    sciPrintText((unsigned char *)str, 5);
  }

  while (1)
    ;
}
