#include "obc_i2c_io.h"
#include "obc_errors.h"
#include "obc_assert.h"
#include "obc_logging.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_semphr.h>
#include <os_task.h>

#include <i2c.h>
#include <sys_common.h>

// The I2C bus to use for the OBC
#define I2C_REG i2cREG1

// Max number of bytes you can send when calling i2cWriteReg
#define I2C_WRITE_REG_MAX_BYTES 32U

// The mutex timeout for the I2C bus
#define I2C_MUTEX_TIMEOUT portMAX_DELAY

STATIC_ASSERT(I2C_REG == i2cREG1, "I2C_REG must be i2cREG1");

static SemaphoreHandle_t i2cMutex = NULL;
static StaticSemaphore_t i2cMutexBuffer;

void initI2CMutex(void) {
    if (i2cMutex == NULL) {
        i2cMutex = xSemaphoreCreateMutexStatic(&i2cMutexBuffer);
    }

    ASSERT(i2cMutex != NULL);
}

obc_error_code_t i2cSendTo(uint8_t sAddr, uint16_t size, uint8_t *buf) {
    ASSERT(i2cMutex != NULL);

    if (buf == NULL || size < 1)
        return OBC_ERR_CODE_INVALID_ARG;

    if (xSemaphoreTake(i2cMutex, I2C_MUTEX_TIMEOUT) == pdTRUE) {
        // As discussed in PR #11, a critical section might not be required
        taskENTER_CRITICAL();

        i2cSetSlaveAdd(I2C_REG, sAddr);
        i2cSetDirection(I2C_REG, I2C_TRANSMITTER);
        i2cSetCount(I2C_REG, size);
        i2cSetMode(I2C_REG, I2C_MASTER);
        i2cSetStop(I2C_REG);
        i2cSetStart(I2C_REG);
        i2cSend(I2C_REG, size, buf);

        taskEXIT_CRITICAL();

        /* Wait for bus to not be busy */
        while(i2cIsBusBusy(I2C_REG));

        /* Wait until Stop is detected */
        while(!i2cIsStopDetected(I2C_REG));

        /* Clear the Stop condition */
        i2cClearSCD(I2C_REG);

        xSemaphoreGive(i2cMutex); // Won't fail because the mutex is taken correctly
        return OBC_ERR_CODE_SUCCESS;
    }

    return OBC_ERR_CODE_MUTEX_TIMEOUT;
}

obc_error_code_t i2cReceiveFrom(uint8_t sAddr, uint16_t size, uint8_t *buf) {
    ASSERT(i2cMutex != NULL);

    if (buf == NULL || size < 1)
        return OBC_ERR_CODE_INVALID_ARG;

    if (xSemaphoreTake(i2cMutex, I2C_MUTEX_TIMEOUT) == pdTRUE) {
        taskENTER_CRITICAL();

        i2cSetSlaveAdd(I2C_REG, sAddr);
        i2cSetDirection(I2C_REG, I2C_RECEIVER);
        i2cSetCount(I2C_REG, size);
        i2cSetMode(I2C_REG, I2C_MASTER);
        i2cSetStop(I2C_REG);
        i2cSetStart(I2C_REG);
        i2cReceive(I2C_REG, size, buf);

        taskEXIT_CRITICAL();

        while(i2cIsBusBusy(I2C_REG));
        while(!i2cIsStopDetected(I2C_REG));

        /* Clear the Stop condition */
        i2cClearSCD(I2C_REG);

        xSemaphoreGive(i2cMutex);
        return OBC_ERR_CODE_SUCCESS;
    }

    return OBC_ERR_CODE_MUTEX_TIMEOUT;
}

obc_error_code_t i2cReadReg(uint8_t sAddr, uint8_t reg, uint8_t *data, uint16_t numBytes) {
    obc_error_code_t errCode;
    
    ASSERT(i2cMutex != NULL);

    if (data == NULL || numBytes < 1)
        return OBC_ERR_CODE_INVALID_ARG;

    RETURN_IF_ERROR_CODE(i2cSendTo(sAddr, 1, &reg));

    RETURN_IF_ERROR_CODE(i2cReceiveFrom(sAddr, numBytes, data));

    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t i2cWriteReg(uint8_t sAddr, uint8_t reg, uint8_t *data, uint8_t numBytes) {
    obc_error_code_t errCode;

    ASSERT(i2cMutex != NULL);

    if (data == NULL || numBytes < 1 || numBytes > I2C_WRITE_REG_MAX_BYTES)
        return OBC_ERR_CODE_INVALID_ARG;
    
    uint8_t dataBuf[I2C_WRITE_REG_MAX_BYTES + 1];
    dataBuf[0] = reg;

    for (int i = 0; i < numBytes; i++) {
        dataBuf[i + 1] = data[i];
    }

    RETURN_IF_ERROR_CODE(i2cSendTo(sAddr, numBytes + 1, dataBuf));
    return OBC_ERR_CODE_SUCCESS;
}
