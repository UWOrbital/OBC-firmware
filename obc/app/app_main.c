#include "obc_logging.h"
#include "obc_sci_io.h"
#include "obc_i2c_io.h"
#include "obc_spi_io.h"
#include "obc_reset.h"
#include "obc_scheduler_config.h"
#include "state_mgr.h"
#include "obc_print.h"

#include <FreeRTOS.h>
#include <os_task.h>
#include <os_semphr.h>

#include <sys_common.h>
#include <sys_core.h>
#include <gio.h>
#include <sci.h>
#include <i2c.h>
#include <spi.h>
#include <can.h>
#include <het.h>

#define SPI_BLOCKING_TIMEOUT pdMS_TO_TICKS(1000)

// This is the stack canary. It should never be overwritten.

void *__stack_chk_guard = (void *)0xDEADBEEF;

uint32_t __stack_chk_guard_init(void);

void __stack_chk_fail(void) { resetSystem(RESET_REASON_STACK_CHECK_FAIL); }
void vSpiMutextOwnerTestFunction();

int main(void) {
  // Run hardware initialization code
  gioInit();
  sciInit();
  i2cInit();
  spiInit();
  canInit();
  hetInit();
  _enable_interrupt_();

  // Initialize bus mutexes
  initSciMutex();
  initI2CMutex();
  initSpiMutex();

  // The state_mgr is the only task running initially.
  // obcSchedulerInitTask(OBC_SCHEDULER_CONFIG_ID_STATE_MGR);
  // obcSchedulerCreateTask(OBC_SCHEDULER_CONFIG_ID_STATE_MGR);

  // Create test task
  xTaskCreate(vSpiMutextOwnerTestFunction,  // Task function
              "SpiMutextTestTask",          // Name of the task
              1000,                         // Stack size in words
              NULL,                         // Task parameter
              1,                            // Task priority
              NULL                          // Task handle
  );

  vTaskStartScheduler();
}

void vSpiMutextOwnerTestFunction() {
  bool passed = true;
  SemaphoreHandle_t spiMutex;

  sciPrintText((unsigned char *)"Testing isSpiBusOwner...\n\r", strlen("Testing isSpiBusOwner...\n\r"), portMAX_DELAY);

  // Get mutex for Spi reg
  getSpiMutex((spiBASE_t *)0xFFF7F400U, &spiMutex);

  // Acquire mutex and test
  if (xQueueTakeMutexRecursive((spiMutex), (SPI_BLOCKING_TIMEOUT)) == pdTRUE) {
    sciPrintText((unsigned char *)"Acquired mutex\n\r", strlen("Acquired mutex\n\r"), portMAX_DELAY);
    if (!isSpiBusOwner(spiMutex)) {
      sciPrintText((unsigned char *)"FAIL: isSpiBusOwner returned FALSE after acquiring mutex\n\r",
                   strlen("FAIL: isSpiBusOwner returned FALSE after acquiring mutex\n\r"), portMAX_DELAY);
      passed = false;
    } else {
      sciPrintText((unsigned char *)"PASS: isSpiBusOwner returned TRUE after acquiring mutex\n\r",
                   strlen("PASS: isSpiBusOwner returned TRUE after acquiring mutex\n\r"), portMAX_DELAY);
    }
  } else {
    sciPrintText((unsigned char *)"Failed to acquire mutex - TIMEOUT\n\r",
                 strlen("Failed to acquire mutex - TIMEOUT\n\r"), portMAX_DELAY);
    passed = false;
  }

  sciPrintText((unsigned char *)"Releasing mutex...\n\r", strlen("Releasing mutex...\n\r"), portMAX_DELAY);

  // Release mutex and test
  if (passed && xQueueGiveMutexRecursive(spiMutex) == pdTRUE) {
    sciPrintText((unsigned char *)"Released mutex\n\r", strlen("Released mutex\n\r"), portMAX_DELAY);
    if (isSpiBusOwner(spiMutex)) {
      sciPrintText((unsigned char *)"FAIL: isSpiBusOwner returned TRUE after releasing mutex\n\r",
                   strlen("FAIL: isSpiBusOwner returned TRUE after releasing mutex\n\r"), portMAX_DELAY);
      passed = false;
    } else {
      sciPrintText((unsigned char *)"PASS: isSpiBusOwner returned FALSE after releasing mutex\n\r",
                   strlen("PASS: isSpiBusOwner returned FALSE after releasing mutex\n\r"), portMAX_DELAY);
    }
  } else {
    sciPrintText((unsigned char *)"Failed to release mutex - TIMEOUT\n\r",
                 strlen("Failed to release mutex - TIMEOUT\n\r"), portMAX_DELAY);
    passed = false;
  }

  if (passed)
    sciPrintText((unsigned char *)"PASSED isSpiBusOwner Test!\n\r", strlen("PASSED isSpiBusOwner Test!\n\r"),
                 portMAX_DELAY);
  else
    sciPrintText((unsigned char *)"FAILED isSpiBusOwner Test!\n\r", strlen("FAILED isSpiBusOwner Test!\n\r"),
                 portMAX_DELAY);

  while (1) {
    vTaskDelay(pdMS_TO_TICKS(1000));  // Delay for 1000ms (1 second)
  }
}
