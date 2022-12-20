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
    sciPrintf("red_init() returned %d\r\n", ret);

    if(ret == 0) {
        ret = red_format("");
        sciPrintf("red_format() returned %d\r\n", ret);
        if(ret == 0) {
            ret = red_mount("");
            sciPrintf("red_mount() returned %d\r\n", ret);
            
            if (ret == 0) {
                vTaskDelay(1000);
                int32_t file = red_open(fname, RED_O_RDWR | RED_O_CREAT);
                sciPrintf("red_open() opened %d\r\n", file);

                ret = red_write(file, "TESTING 1...2...3\r\n", strlen("TESTING 1...2...3\r\n"));
                sciPrintf("red_write() returned %d\r\n", ret);

                ret = red_close(file);
                sciPrintf("red_close() returned %d\r\n", ret);

                file = red_open(fname, RED_O_RDWR);
                sciPrintf("red_open() opened %d\r\n", file);

                unsigned char readBuf[100] = {0};
                ret = red_read(file, readBuf, 100);
                sciPrintf("red_read() returned %d\r\n", ret);

                if (ret > 0)
                    sciPrintText(readBuf, ret);

                ret = red_close(file);
                sciPrintf("red_close() returned %d\r\n", ret);

                ret = red_unlink(fname);
                sciPrintf("red_unlink() returned %d\r\n", ret);

            }
        }
    }
    
    while(1);
}

int main(void) {
    sciInit();
    spiInit();
    
    initSciMutex();
    initSpiMutex();

    xTaskCreateStatic(vTask1, "RelianceEdgeDemo", 1024, NULL, 1, taskStack, &taskBuffer);

    vTaskStartScheduler();

    while (1);
}
