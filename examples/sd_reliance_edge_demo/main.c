#include "obc_spi_io.h"
#include "obc_sci_io.h"

#include <FreeRTOS.h>
#include <os_task.h>

#include <sys_common.h>
#include <sci.h>
#include <spi.h>

#include <redposix.h>
#include <string.h>

static StaticTask_t taskBuffer;
static StackType_t taskStack[1024];

void vTask1(void *pvParameters) {
    const char * fname = "/demo.txt";
    int32_t ret; 

    ret = red_init();
    if (ret != 0) {
        sciPrintf("red_init() returned %d\r\n", ret);
        while (1);
    }

    ret = red_format("");
    if (ret != 0) {
        sciPrintf("red_format() returned %d\r\n", ret);
        while (1);
    }

    ret = red_mount("");
    if (ret != 0) {
        sciPrintf("red_mount() returned %d\r\n", ret);
        while (1);
    }

    sciPrintf("Reliance Edge initialized and mounted\r\n");

    int32_t file = red_open(fname, RED_O_RDWR | RED_O_CREAT);
    if (file < 0) {
        sciPrintf("red_open() returned %d\r\n", file);
        while (1);
    }
    sciPrintf("Successfully opened %s\r\n", fname);
    
    ret = red_write(file, "TESTING 1...2...3\r\n", strlen("TESTING 1...2...3\r\n"));
    if (ret < 0) {
        sciPrintf("red_write() returned %d\r\n", ret);
        while (1);
    }
    sciPrintf("Successfully wrote to %s\r\n", fname);

    ret = red_close(file);
    if (ret < 0) {
        sciPrintf("red_close() returned %d\r\n", ret);
        while (1);
    }
    sciPrintf("Successfully closed %s\r\n", fname);

    file = red_open(fname, RED_O_RDWR);
    if (file < 0) {
        sciPrintf("red_open() returned %d\r\n", file);
        while (1);
    }
    sciPrintf("Successfully opened %s\r\n", fname);

    unsigned char readBuf[100] = {0};
    ret = red_read(file, readBuf, 100);
    if (ret < 0) {
        sciPrintf("red_read() returned %d\r\n", ret);
        while (1);
    }
    sciPrintf("Successfully read from %s\r\n", fname);

    sciPrintf("Text read from %s: ", fname);
    sciPrintText(readBuf, ret);

    ret = red_close(file);
    if (ret < 0) {
        sciPrintf("red_close() returned %d\r\n", ret);
        while (1);
    }
    sciPrintf("Successfully closed %s\r\n", fname);

    ret = red_unlink(fname);
    if (ret < 0) {
        sciPrintf("red_unlink() returned %d\r\n", ret);
    } else {
        sciPrintf("Successfully unlinked %s\r\n", fname);
    }
    
    while(1);
}

int main(void) {
    sciInit();
    spiInit();
    
    initSciMutex();
    initSpiMutex();

    sciPrintf("Starting Reliance Edge Demo\r\n");

    xTaskCreateStatic(vTask1, "RelianceEdgeDemo", 1024, NULL, 1, taskStack, &taskBuffer);

    vTaskStartScheduler();

    while (1);
}
