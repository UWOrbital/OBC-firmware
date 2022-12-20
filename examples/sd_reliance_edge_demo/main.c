#include "obc_spi_io.h"
#include "obc_sci_io.h"

#include <FreeRTOS.h>
#include <os_task.h>

#include <sys_common.h>
#include <sci.h>
#include <spi.h>

#include <redposix.h>

static StaticTask_t taskBuffer;
static StackType_t taskStack[1024];

void vTask1(void *pvParameters) {
    int32_t ret;

    ret = red_init();
    sciPrintf(scilinREG, "red_init() returned %d\r\n", ret);

    if(ret == 0) {
        ret = red_format("");
        sciPrintf(scilinREG, "red_format() returned %d\r\n", ret);
        if(ret == 0) {
            ret = red_mount("");
            sciPrintf(scilinREG, "red_mount() returned %d\r\n", ret);
            
            if (ret == 0) {
                vTaskDelay(1000);
                int32_t file = red_open("/reliance.txt", RED_O_RDWR | RED_O_CREAT);
                sciPrintf(scilinREG, "red_open() opened %d\r\n", file);

                unsigned char buf_wr[] = "Hello World!\r\n";
                ret = red_write(file, buf_wr, sizeof(buf_wr));
                sciPrintf(scilinREG, "red_write() returned %d\r\n", ret);

                ret = red_close(file);
                sciPrintf(scilinREG, "red_close() returned %d\r\n", ret);

                file = red_open("/reliance.txt", RED_O_RDWR);
                sciPrintf(scilinREG, "red_open() opened %d\r\n", file);

                unsigned char buf[20] = {0};
                ret = red_read(file, buf, 15);
                sciPrintf(scilinREG, "red_read() returned %d\r\n", ret);

                printTextSci(scilinREG, buf, 14);
                sciPrintf(scilinREG, "\r\n", NULL);

                ret = red_close(file);
                sciPrintf(scilinREG, "red_close() returned %d\r\n", ret);

                ret = red_unlink("/reliance.txt");
                sciPrintf(scilinREG, "red_unlink() returned %d\r\n", ret);

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
