#include "obc_sci_io.h"
#include "obc_spi_io.h"
#include "obc_errors.h"
#include "obc_persistent_data_config.h"
#include "obc_persistent_store.h"
#include "fram.h"

#include <FreeRTOS.h>
#include <os_task.h>

#include <sys_common.h>
#include <sci.h>
#include <spi.h>

static StaticTask_t taskBuffer;
static StackType_t taskStack[1024];

void vTask1(void *pvParameters) {
    obc_error_code_t errCode;
    sciPrintf("Persist Demo\r\n");

    fram_time_data_t timeData = {0};
    timeData.unix_time = 0x12345678;

    errCode = setPersistentTimeData(timeData);
    if (errCode != OBC_ERR_CODE_SUCCESS) {
        sciPrintf("Error setting time data: %d\r\n", errCode);
    }

    fram_time_data_t readTimeData = {0};
    errCode = getPersistentTimeData(&readTimeData);
    if (errCode != OBC_ERR_CODE_SUCCESS) {
        sciPrintf("Error getting time data: %d\r\n", errCode);
    } else {
        sciPrintf("Time data: %x\r\n", readTimeData.unix_time);
    }

    // Corrupt time data
    uint8_t corrupt = 0xFF;
    framWrite(0x9, &corrupt, 1);

    errCode = getPersistentTimeData(&readTimeData);
    if (errCode != OBC_ERR_CODE_SUCCESS) {
        if (errCode == OBC_ERR_CODE_PERSISTENT_CORRUPTED) {
            sciPrintf("FRAM is corrupt\r\n");
        } else {
            sciPrintf("Error getting time data: %d\r\n", errCode);
        }
    } else {
        sciPrintf("Time data: %x\r\n", readTimeData.unix_time);
    }

    while (1);
}

int main(void) {
    sciInit();
    spiInit();
    
    initSciMutex();
    initSpiMutex();

    xTaskCreateStatic(vTask1, "Demo", 1024, NULL, 1, taskStack, &taskBuffer);

    vTaskStartScheduler();

    while (1);
}
