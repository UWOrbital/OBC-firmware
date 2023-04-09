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

obc_error_code_t canSendMessage(canBASE_t *canReg, uint32_t messageBox, const uint8_t *txData) {
    ASSERT(canTXMutex != NULL);

    obc_error_code_t status = OBC_ERR_CODE_UNKNOWN;

    if(sizeof(*txData) > 8) { /* Check that tx data is within 8 bytes */
        return OBC_ERR_CODE_CAN_TX_DATA_OVERSIZE;
    }
    if(txData == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    if(xSemaphoreTake(canTXMutex, portMAX_DELAY) == pdTRUE) {
        taskENTER_CRITICAL(); 

        if(canTransmit(canReg, messageBox, txData)) {
            status = OBC_ERR_CODE_SUCCESS;
        }
        else {
            status = OBC_ERR_CODE_CAN_FAILURE;
        }

        taskEXIT_CRITICAL();
        xSemaphoreGive(canTXMutex);
    }

    return status;
}

obc_error_code_t canGetMessage(canBASE_t *canReg, uint32_t messageBox, uint8_t *rxData) {
    ASSERT(canRXMutex != NULL);

    obc_error_code_t status = OBC_ERR_CODE_UNKNOWN;

    while(!canIsRxMessageArrived(canReg, messageBox)); /* Poll for RX CAN message */

    if(xSemaphoreTake(canRXMutex, portMAX_DELAY) == pdTRUE) {
        taskENTER_CRITICAL(); 

        if(canGetData(canReg, messageBox, rxData) == 1) {
            status = OBC_ERR_CODE_SUCCESS;
        }
        else if(canGetData(canReg, messageBox, rxData) == 3) {
            status = OBC_ERR_CODE_CAN_MESSAGE_LOST;
        }
        else {
            status = OBC_ERR_CODE_CAN_FAILURE;
        }

        taskEXIT_CRITICAL();
        xSemaphoreGive(canRXMutex);
    }
    else {
        status = OBC_ERR_CODE_MUTEX_TIMEOUT;
    }
    
    return status;
}