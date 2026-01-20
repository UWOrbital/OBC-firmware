#include "obc_logging.h"
#include "obc_sci_io.h"
#include "obc_spi_io.h"
#include "cc1120.h"

#include "FreeRTOS.h"
#include "os_task.h"
#include "os_portable.h"

#include <sys_common.h>
#include <gio.h>
#include <sci.h>
#include <spi.h>
#include <stdio.h>
#include <string.h>

static TaskHandle_t testTaskHandle = NULL;
static StaticTask_t testTaskBuffer;
static StackType_t testTaskStack[1024U];

void initTestTask(void);
static void vTestTask(void* pvParameters);

void initTestTask(void) {
  ASSERT((testTaskStack != NULL) && (&testTaskBuffer != NULL));
  if (testTaskHandle == NULL) {
    testTaskHandle = xTaskCreateStatic(vTestTask, "test task", 1024U, NULL, 1U, testTaskStack, &testTaskBuffer);
  }
}

static void vTestTask(void* pvParameters) {
  float temperature;
  obc_error_code_t err;
  char tempStr[32];

  // Initialize CC1120
  err = cc1120Init();
  if (err != OBC_ERR_CODE_SUCCESS) {
    printTextSci(scilinREG, "cc1120Init failed\r\n", 20);
    return;
  }

  // Read temperature every 5 seconds
  while (1) {
    err = cc1120ReadTemp(&temperature);
    if (err == OBC_ERR_CODE_SUCCESS) {
      snprintf(tempStr, sizeof(tempStr), "CC1120 temperature: %.2f C\r\n", temperature);
      printTextSci(scilinREG, tempStr, strlen(tempStr));
    } else {
      printTextSci(scilinREG, "Failed to read temperature\r\n", 29);
    }
    vTaskDelay(pdMS_TO_TICKS(5000));
  }
}

int main(void) {
  initSciMutex();
  gioInit();
  sciInit();
  spiInit();
  adcInit();

  // Initialize logger
  // initLogger();
  // logSetLevel(LOG_DEBUG);

  // Initialize bus mutexes
  initSciMutex();
  initSpiMutex();

  initTestTask();

  vTaskStartScheduler();
}