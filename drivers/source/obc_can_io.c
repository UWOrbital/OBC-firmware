#include "obc_can_io.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_semphr.h>
#include <os_task.h>

#include <can.h>
#include <sys_common.h>

static SemaphoreHandle_t canTXMutex = NULL;
static SemaphoreHandle_t canRXMutex = NULL;
static StaticSemaphore_t canMutexBuffer;

void initCANMutex(void) {
    if(canTXMutex == NULL) {
        canTXMutex = xSemaphoreCreateMutexStatic(&canMutexBuffer);
    }
    if(canRXMutex == NULL) {
        canRXMutex = xSemaphoreCreateMutexStatic(&canMutexBuffer);
    }

    configASSERT(canTXMutex);
    configASSERT(canRXMutex);
}

uint8_t canSendMessage(canBASE_t *canReg, uint32_t messageBox, const uint8_t *txData) {
    ASSERT(canTXMutex != NULL);

    uint8_t status = 0;

    if(sizeof(*txData) > 0x08) { /* Check that tx data is within 8 bytes (assuming standard CAN packet) */
        return status;
    }

    if(xSemaphoreTake(canTXMutex, portMAX_DELAY) == pdTRUE) {
        taskENTER_CRITICAL(); 

        status = canTransmit(canReg, messageBox, txData);

        taskEXIT_CRITICAL();
        xSemaphoreGive(canTXMutex);
    }

    return status;
}

uint8_t canGetMessage(canBASE_t *canReg, uint32_t messageBox, const uint8_t *rxData) {
    ASSERT(canRXMutex != NULL);

    uint8_t status = 0;

    while(!canIsRxMessageArrived(canReg, messageBox)); /* Poll for RX CAN message */

    if(xSemaphoreTake(canRXMutex, portMAX_DELAY) == pdTRUE) {
        taskENTER_CRITICAL(); 

        status = canGetData(canReg, messageBox, rxData);

        taskEXIT_CRITICAL();
        xSemaphoreGive(canRXMutex);
    }
    
    return status;
}