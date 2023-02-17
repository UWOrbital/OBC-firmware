#include "obc_can_io.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_semphr.h>
#include <os_task.h>

#include <can.h>
#include <sys_common.h>

static SemaphoreHandle_t canMutex = NULL;
static StaticSemaphore_t canMutexBuffer;

void initCANMutex(void) {
    if(canMutex == NULL) {
        canMutex = xSemaphoreCreateMutexStatic(&canMutexBuffer);
    }

    configASSERT(canMutex);
}

uint8_t canSendMessage(canBASE_t *canReg, uint32_t messageBox, const uint8_t *txData) {
    ASSERT(canMutex != NULL);

    uint8_t status = 0;

    // Check that tx data is within 8 bytes (assuming standard CAN packet)

    if(xSemaphoreTake(canMutex, portMAX_DELAY) == pdTRUE) {
        taskENTER_CRITICAL(); 

        status = canTransmit(canReg, messageBox, txData);

        taskEXIT_CRITICAL();
    }

    return status;
}

uint8_t canGetMessage(canBASE_t *canReg, uint32_t messageBox, const uint8_t *rxData) {
    while(!canIsRxMessageArrived(canReg, messageBox)); /* Poll for RX CAN message */
    
    return canGetData(canReg, messageBox, rxData);
}