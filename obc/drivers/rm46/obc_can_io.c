#include "obc_can_io.h"
#include "obc_errors.h"
#include "obc_assert.h"
#include "obc_logging.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_semphr.h>
#include <os_task.h>

#include <can.h>
#include <sys_common.h>

static SemaphoreHandle_t canTXMutex = NULL;
static SemaphoreHandle_t canRXMutex = NULL;
static StaticSemaphore_t canTXMutexBuffer;
static StaticSemaphore_t canRXMutexBuffer;

void initCanMutex(void)
{
    if (canTXMutex == NULL) {
        canTXMutex = xSemaphoreCreateMutexStatic( &canTXMutexBuffer );
    }
    if(canRXMutex) {
        canRXMutex = xSemaphoreCreateMutexStatic( &canRXMutexBuffer );
    }

    ASSERT(canTXMutex != NULL);
    ASSERT(canRXMutex != NULL);
}

obc_error_code_t canSendMessage(canBASE_t *canReg, uint32_t messageBox, const uint8_t *txData) {
    ASSERT(canTXMutex != NULL);

    if(xSemaphoreTake(canTXMutex, portMAX_DELAY) == pdTRUE) {
        if(!canTransmit(canReg, messageBox, txData)) {
            LOG_ERROR_CODE(OBC_ERR_CODE_CAN_SEND_FAILURE);
            return OBC_ERR_CODE_CAN_SEND_FAILURE
        }

        xSemaphoreGive(canTXMutex);
    }

    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t canGetMessage(canBASE_t *canReg, uint32_t messageBox, const uint8_t *rxData) {
    ASSERT(canRXMutex);

    obc_error_code_t ret = OBC_ERR_CODE_CAN_RECV_FAILURE;

    while(!canIsRxMessageArrived(canReg, messageBox)); /* Poll for RX CAN message */

    if(xSemaphoreTake(canRXMutex, portMAX_DELAY) == pdTRUE) {
        uint32 
        ret = canGetData(canReg, messageBox, rxData);
        if(ret == 0) {
            LOG_ERROR_CODE(OBC_ERR_CODE_CAN_NO_RECV);
            return OBC_ERR_CODE_CAN_NO_RECV;
        }
        else if(ret == 3) {
            LOG_ERROR_CODE(OBC_ERR_CODE_CAN_RECV_FAILURE);
            return OBC_ERR_CODE_CAN_RECV_FAILURE;
        }

        xSemaphoreGive(canRXMutex);
    }

    return OBC_ERR_CODE_SUCCESS;
}