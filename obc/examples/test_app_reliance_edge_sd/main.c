#include "obc_spi_io.h"
#include "obc_sci_io.h"

#include <FreeRTOS.h>
#include <os_task.h>

#include <sys_common.h>
#include <sci.h>
#include <spi.h>

#include <redposix.h>
#include <string.h>

#define UART_MUTEX_BLOCK_TIME portMAX_DELAY

static StaticTask_t taskBuffer;
static StackType_t taskStack[1024];

void vTask1(void *pvParameters) {
  const char *fname = "/demo.txt";
  int32_t ret;

  ret = red_init();
  if (ret != 0) {
    sciPrintf("red_init() returned %d\r\n",scilinREG, ret);
    while (1)
      ;
  }

  ret = red_format("");
  if (ret != 0) {
    sciPrintf("red_format() returned %d\r\n",scilinREG, ret);
    while (1)
      ;
  }

  ret = red_mount("");
  if (ret != 0) {
    sciPrintf("red_mount() returned %d\r\n",scilinREG, ret);
    while (1)
      ;
  }

  sciPrintf("Reliance Edge initialized and mounted\r\n", scilinREG);

  REDSTATFS statfs;
  red_statvfs("", &statfs);
  sciPrintf("Disk Info - Total Blocks: %d, Free Blocks: %d\r\n", scilinREG, statfs.f_blocks, statfs.f_bfree);

  int32_t file = red_open(fname, RED_O_WRONLY | RED_O_CREAT);
  if (file < 0) {
    sciPrintf("red_open() returned %d\r\n", scilinREG, file);
    while (1)
      ;
  }
  sciPrintf("Successfully opened %s\r\n", scilinREG, fname);

  ret = red_write(file, "TESTING 1...2...3\r\n", strlen("TESTING 1...2...3\r\n"));
  if (ret < 0) {
    sciPrintf("red_write() returned %d\r\n", scilinREG, ret);
    while (1)
      ;
  }
  sciPrintf("Successfully wrote to %s\r\n", scilinREG, fname);

  ret = red_close(file);
  if (ret < 0) {
    sciPrintf("red_close() returned %d\r\n", scilinREG, ret);
    while (1);
  }
  sciPrintf("Successfully closed %s\r\n", scilinREG, fname);

  // Open the file again, this time in append mode
  file = red_open(fname, RED_O_RDWR | RED_O_APPEND);
  if (file < 0) {
    sciPrintf("red_open() returned %d\r\n", scilinREG, file);
    while (1)
      ;
  }
  sciPrintf("Successfully opened %s\r\n", scilinREG, fname);

  // Append some text to the file
  ret = red_write(file, "TESTING 3...2...1\r\n", strlen("TESTING 3...2...1\r\n"));
  if (ret < 0) {
    sciPrintf("red_write() returned %d\r\n", scilinREG, ret);
    while (1)
      ;
  }
  sciPrintf("Successfully wrote to %s\r\n", scilinREG, fname);

  // Since we want to read the file from the beginning, we need to seek to the beginning
  // Alternatively, just use red_pread() instead of red_read()
  ret = red_lseek(file, 0, RED_SEEK_SET);
  if (ret < 0) {
    sciPrintf("red_lseek() returned %d\r\n", scilinREG, ret);
    while (1)
      ;
  }
  sciPrintf("Successfully seeked to beginning of %s\r\n", scilinREG, fname);

  unsigned char readBuf[100] = {0};
  ret = red_read(file, readBuf, 100);
  if (ret < 0) {
    sciPrintf("red_read() returned %d\r\n", scilinREG, ret);
    while (1)
      ;
  }
  sciPrintf("Successfully read from %s\r\n", scilinREG, fname);

  sciPrintf("Text read from %s: \r\n", scilinREG, fname);
  sciPrintText(readBuf, ret, UART_MUTEX_BLOCK_TIME, scilinREG);

  ret = red_close(file);
  if (ret < 0) {
    sciPrintf("red_close() returned %d\r\n", scilinREG, ret);
    while (1)
      ;
  }
  sciPrintf("Successfully closed %s\r\n", scilinREG, fname);

  ret = red_unlink(fname);
  if (ret < 0) {
    sciPrintf("red_unlink() returned %d\r\n", scilinREG, ret);
  } else {
    sciPrintf("Successfully deleted %s\r\n", scilinREG, fname);
  }

  while (1)
    ;
}

int main(void) {
  sciInit();
  spiInit();

  initSciMutex();
  initSpiMutex();

  sciPrintf("Starting Reliance Edge Demo\r\n", scilinREG);

  xTaskCreateStatic(vTask1, "RelianceEdgeDemo", 1024, NULL, 1, taskStack, &taskBuffer);

  vTaskStartScheduler();

  while (1)
    ;
}
