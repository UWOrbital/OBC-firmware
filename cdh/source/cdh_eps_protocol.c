#include "cdh_eps_protocol.h"
#include "cdh_eps_protocol_id.h"
#include "cdh_eps_callbacks.h"
#include "obc_can_io.h"
#include "obc_task_config.h"
#include "obc_errors.h"
#include "obc_logging.h"

#include <FreeRTOS.h>
#include <sys_common.h>
#include <os_task.h>
#include <os_queue.h>

#define CDH_EPS_QUEUE_LENGTH 25
#define CDH_EPS_QUEUE_ITEM_SIZE sizeof(cdh_eps_queue_msg_t)

static TaskHandle_t cdhepsProtocolTaskHandle;
static StaticTask_t cdhepsProtocolTaskBuffer;
static StackType_t cdhepsProtocolStack[CDH_EPS_STACK_SIZE];

static QueueHandle_t cdhepsTxQueueHandle;
static StaticQueue_t cdhepsTxQueue;
static uint8_t cdhepsTxQueueStack[CDH_EPS_QUEUE_LENGTH*CDH_EPS_QUEUE_ITEM_SIZE];

static QueueHandle_t cdhepsRxQueueHandle;
static StaticQueue_t cdhepsRxQueue;
static uint8_t cdhepsRxQueueStack[CDH_EPS_QUEUE_LENGTH*CDH_EPS_QUEUE_ITEM_SIZE];

static const rx_callback_t rxCallbacks[] = {
    [CMD_SUBSYS_SHUTDDOWN] = subsysShutdownCmdCallback,
    [CMD_HEARTBEAT] = heartbeatCmdCallback,
    [CMD_GET_TELEMETRY] = getTelemetryCmdCallback,
    [RESP_SUBSYS_SHUTDDOWN_ACK] = respSubsysShutdownAckCallback,
    [RESP_HEARTBEAT_ACK] = respHeartbeatAckCallback
};
/**
 * @brief Task that manages the CAN TX and RX between CDH and EPS
 */
static void cdhepsProtocolTask(void *pvParameters);

void initCDHEPSProtocol(void) {
    ASSERT( (cdhepsProtocolStack != NULL) && (&cdhepsProtocolTaskBuffer != NULL) );
    if(cdhepsProtocolTaskHandle == NULL) {
        cdhepsProtocolTaskHandle = xTaskCreateStatic(
            cdhepsProtocolTask,
            CDH_EPS_NAME,
            CDH_EPS_STACK_SIZE,
            NULL,
            CDH_EPS_PRIORITY,
            cdhepsProtocolStack,
            &cdhepsProtocolTaskBuffer
        );
    }

    ASSERT( (cdhepsTxQueueStack != NULL) && (&cdhepsTxQueue != NULL) );
    if(cdhepsTxQueueHandle == NULL) {
        cdhepsTxQueueHandle = xQueueCreateStatic(
            CDH_EPS_QUEUE_LENGTH,
            CDH_EPS_QUEUE_ITEM_SIZE,
            cdhepsTxQueueStack,
            &cdhepsTxQueue
        );
    }

    ASSERT( (cdhepsRxQueueStack != NULL) && (&cdhepsRxQueue != NULL) );
    if(cdhepsRxQueueHandle == NULL) {
        cdhepsRxQueueHandle = xQueueCreateStatic(
            CDH_EPS_QUEUE_LENGTH,
            CDH_EPS_QUEUE_ITEM_SIZE,
            cdhepsRxQueueStack,
            &cdhepsRxQueue
        );
    }
}

obc_error_code_t sendToCDHEPSTxQueue(cdh_eps_queue_msg_t *msg) {
    if(msg == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    if(cdhepsTxQueueHandle == NULL) {
        return OBC_ERR_CODE_INVALID_STATE;
    }

    if(xQueueSend(cdhepsTxQueueHandle, (void *) msg, portMAX_DELAY) == pdPASS) {
        return OBC_ERR_CODE_SUCCESS;
    }

    return OBC_ERR_CODE_QUEUE_FULL;
}

obc_error_code_t sendToCDHEPSRxQueue(cdh_eps_queue_msg_t *msg) {
    if(msg == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }
    
    if(cdhepsRxQueueHandle == NULL) {
        return OBC_ERR_CODE_INVALID_STATE;
    }

    if(xQueueSend(cdhepsRxQueueHandle, (void *) msg, portMAX_DELAY) == pdPASS) {
        return OBC_ERR_CODE_SUCCESS;
    }

    return OBC_ERR_CODE_QUEUE_FULL;
}

/* Process queued messages to transmit to EPS*/
static void processTxMessages(void) {
    cdh_eps_queue_msg_t msg;

    if(xQueueReceive(cdhepsTxQueueHandle, &msg, portMAX_DELAY) == pdPASS) {
        uint32_t txMessageBox;

        switch (msg.cmd.id) {
            case CMD_SUBSYS_SHUTDDOWN: /* Subsystem shutdown command */
                txMessageBox = canMESSAGE_BOX1;
                break;
            case CMD_HEARTBEAT: /* Heartbeat command */
                txMessageBox = canMESSAGE_BOX3;
                break;
            case CMD_GET_TELEMETRY: /* Get telemetry command */
                txMessageBox = canMESSAGE_BOX4;
                break;
            case RESP_SUBSYS_SHUTDDOWN_ACK: /* Subsystem shutdown ACK */
                txMessageBox = canMESSAGE_BOX6;
                break;
            case RESP_HEARTBEAT_ACK:
                txMessageBox = canMESSAGE_BOX7;
                break;
            default:
                break;
        }

        //uint8_t *txData = &msg;
        canSendMessage(canREG1, txMessageBox, (uint8_t *) &msg); /* Send tx message to eps over CAN */
    }
}

/* Process recieved messages from EPS */
static void processRxMessages(void) {
    cdh_eps_queue_msg_t msg;
    
    /* Process recieved messages from EPS */
    if(xQueueReceive(cdhepsRxQueueHandle, &msg, portMAX_DELAY) == pdPASS) {
        switch (msg.cmd.id) {
           case CMD_SUBSYS_SHUTDDOWN:
                subsysShutdownCmdCallback(&msg);
                break;
            case CMD_HEARTBEAT:
                heartbeatCmdCallback(&msg);
                break;
            case CMD_GET_TELEMETRY:
                getTelemetryCmdCallback(&msg);
                break;
           case RESP_SUBSYS_SHUTDDOWN_ACK:
                respSubsysShutdownAckCallback(&msg);
                break;
            case RESP_HEARTBEAT_ACK:
                respHeartbeatAckCallback(&msg);
                break;
        default:
            break;
        }
    }
}

static void cdhepsProtocolTask(void *pvParameters) {

    while(1) {
        processTxMessages();
        processRxMessages();
    }
}