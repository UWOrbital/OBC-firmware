#include "obc_i2c_io.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_semphr.h>
#include <os_task.h>

#include <i2c.h>
#include <sys_common.h>

static SemaphoreHandle_t i2cMutex = NULL;
static StaticSemaphore_t i2cMutexBuffer;

void initI2CMutex(void) {
    if (i2cMutex == NULL) {
        i2cMutex = xSemaphoreCreateMutexStatic(&i2cMutexBuffer);
    }

    configASSERT(i2cMutex);
}

uint8_t i2cSendTo(uint8_t sAddr, uint16_t size, void *buf) {
    ASSERT(i2cMutex != NULL);
    if (xSemaphoreTake(i2cMutex, portMAX_DELAY) == pdTRUE) {
        // As discussed in PR #11, a critical section might not be required
        taskENTER_CRITICAL();

        i2cSetSlaveAdd(i2cREG1, sAddr);
        i2cSetDirection(i2cREG1, I2C_TRANSMITTER);
        i2cSetCount(i2cREG1, size);
        i2cSetMode(i2cREG1, I2C_MASTER);
        i2cSetStop(i2cREG1);
        i2cSetStart(i2cREG1);
        i2cSend(i2cREG1, size, buf);

        taskEXIT_CRITICAL();

        /* Wait for bus to not be busy */
        while(i2cIsBusBusy(i2cREG1) == true);

        /* Wait until Stop is detected */
        while(i2cIsStopDetected(i2cREG1) == 0);

        /* Clear the Stop condition */
        i2cClearSCD(i2cREG1);

        xSemaphoreGive(i2cMutex);
        return 1;
    }

    return 0;
}

uint8_t i2cReceiveFrom(uint8_t sAddr, uint16_t size, void *buf) {
    ASSERT(i2cMutex != NULL);
    if (xSemaphoreTake(i2cMutex, portMAX_DELAY) == pdTRUE) {
        taskENTER_CRITICAL();

        i2cSetSlaveAdd(i2cREG1, sAddr);
        i2cSetDirection(i2cREG1, I2C_RECEIVER);
        i2cSetCount(i2cREG1, size);
        i2cSetMode(i2cREG1, I2C_MASTER);
        i2cSetStop(i2cREG1);
        i2cSetStart(i2cREG1);
        i2cReceive(i2cREG1, size, buf);

        taskEXIT_CRITICAL();

        while(i2cIsBusBusy(i2cREG1) == true);
        while(i2cIsStopDetected(i2cREG1) == 0);

        /* Clear the Stop condition */
        i2cClearSCD(i2cREG1);

        xSemaphoreGive(i2cMutex);
        return 1;
    }

    return 0;
}

uint8_t i2cReadReg(uint8_t sAddr, uint8_t reg, uint8_t *data, uint16_t numBytes) {
    if (i2cSendTo(sAddr, 1, &reg) == 0) {
        return 0;
    }

    if (i2cReceiveFrom(sAddr, numBytes, data) == 0) {
        return 0;
    }
    return 1;
}

uint8_t i2cWriteReg(uint8_t sAddr, uint8_t reg, uint8_t *data, uint8_t numBytes) {
    uint8_t data_buf[numBytes + 1];
    data_buf[0] = reg;
    for ( int i = 0; i < numBytes; i++ ) {
        data_buf[i + 1] = data[i];
    }
    return i2cSendTo(sAddr, numBytes + 1, &data_buf);
}  