#include "cdh_eps_protocol.h"
#include "cdh_eps_protocol_id.h"
#include "obc_can_io.h"
#include "obc_task_config.h"
#include "obc_errors.h"
#include "obc_logging.h"

#include <FreeRTOS.h>
#include <sys_common.h>
#include <os_task.h>
#include <os_queue.h>

#define CDH_EPS_CMD_TX_QUEUE_LENGTH 25
#define CDH_EPS_CMD_TX_QUEUE_ITEM_SIZE sizeof(cdh_eps_cmd_msg_t)

#define CDH_EPS_CMD_RX_QUEUE_LENGTH 25
#define CDH_EPS_CMD_RX_QUEUE_ITEM_SIZE sizeof(cdh_eps_cmd_msg_t)

#define CDH_EPS_RESP_TX_QUEUE_LENGTH 25
#define CDH_EPS_RESP_TX_QUEUE_ITEM_SIZE sizeof(cdh_eps_resp_msg_t)

#define CDH_EPS_RESP_RX_QUEUE_LENGTH 25
#define CDH_EPS_RESP_RX_QUEUE_ITEM_SIZE sizeof(cdh_eps_resp_msg_t)

static TaskHandle_t cdhepsProtocolTaskHandle;
static StaticTask_t cdhepsProtocolTaskBuffer;
static StackType_t cdhepsProtocolStack[CDH_EPS_STACK_SIZE];

static QueueHandle_t cdhepsCmdTxQueueHandle;
static StaticQueue_t cdhepsCmdTxQueue;
static uint8_t cdhepsCmdTxQueueStack[CDH_EPS_CMD_TX_QUEUE_LENGTH*CDH_EPS_CMD_TX_QUEUE_ITEM_SIZE];

static QueueHandle_t cdhepsCmdRxQueueHandle;
static StaticQueue_t cdhepsCmdRxQueue;
static uint8_t cdhepsCmdRxQueueStack[CDH_EPS_CMD_RX_QUEUE_LENGTH*CDH_EPS_CMD_RX_QUEUE_ITEM_SIZE];

static QueueHandle_t cdhepsRespTxQueueHandle;
static StaticQueue_t cdhepsRespTxQueue;
static uint8_t cdhepsRespTxQueueStack[CDH_EPS_RESP_TX_QUEUE_LENGTH*CDH_EPS_RESP_TX_QUEUE_ITEM_SIZE];

static QueueHandle_t cdhepsRespRxQueueHandle;
static StaticQueue_t cdhepsRespRxQueue;
static uint8_t cdhepsRespRxQueueStack[CDH_EPS_RESP_RX_QUEUE_LENGTH*CDH_EPS_RESP_RX_QUEUE_ITEM_SIZE];

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

    ASSERT( (cdhepsCmdTxQueueStack != NULL) && (&cdhepsCmdTxQueue != NULL) );
    if(cdhepsCmdTxQueueHandle == NULL) {
        cdhepsCmdTxQueueHandle = xQueueCreateStatic(
            CDH_EPS_CMD_TX_QUEUE_LENGTH,
            CDH_EPS_CMD_TX_QUEUE_ITEM_SIZE,
            cdhepsCmdTxQueueStack,
            &cdhepsCmdTxQueue
        );
    }

    ASSERT( (cdhepsCmdRxQueueStack != NULL) && (&cdhepsCmdRxQueue != NULL) );
    if(cdhepsCmdRxQueueHandle == NULL) {
        cdhepsCmdRxQueueHandle = xQueueCreateStatic(
            CDH_EPS_CMD_RX_QUEUE_LENGTH,
            CDH_EPS_CMD_RX_QUEUE_ITEM_SIZE,
            cdhepsCmdRxQueueStack,
            &cdhepsCmdRxQueue
        );
    }

    ASSERT( (cdhepsRespTxQueueStack != NULL) && (&cdhepsRespTxQueue != NULL) );
    if(cdhepsRespTxQueueHandle == NULL) {
        cdhepsRespTxQueueHandle = xQueueCreateStatic(
            CDH_EPS_RESP_TX_QUEUE_LENGTH,
            CDH_EPS_RESP_TX_QUEUE_ITEM_SIZE,
            cdhepsRespTxQueueStack,
            &cdhepsRespTxQueue
        );
    }

    ASSERT( (cdhepsRespRxQueueStack != NULL) && (&cdhepsRespRxQueue != NULL) );
    if(cdhepsRespRxQueueHandle == NULL) {
        cdhepsRespRxQueueHandle = xQueueCreateStatic(
            CDH_EPS_RESP_RX_QUEUE_LENGTH,
            CDH_EPS_RESP_RX_QUEUE_ITEM_SIZE,
            cdhepsRespRxQueueStack,
            &cdhepsRespRxQueue
        );
    }
}

obc_error_code_t sendToCDHEPSCmdTxQueue(cdh_eps_cmd_msg_t *cmd) {
    if(cdhepsCmdTxQueueHandle == NULL) {
        return OBC_ERR_CODE_INVALID_STATE;
    }

    if(cmd == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    if(xQueueSend(cdhepsCmdTxQueueHandle, (void *) cmd, portMAX_DELAY) == pdPASS) {
        return OBC_ERR_CODE_SUCCESS;
    }

    return OBC_ERR_CODE_QUEUE_FULL;
}

obc_error_code_t sendToCDHEPSCmdRxQueue(cdh_eps_cmd_msg_t *cmd) {
    if(cdhepsCmdRxQueueHandle == NULL) {
        return OBC_ERR_CODE_INVALID_STATE;
    }

    if(cmd == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    if(xQueueSend(cdhepsCmdRxQueueHandle, (void *) cmd, portMAX_DELAY) == pdPASS) {
        return OBC_ERR_CODE_SUCCESS;
    }

    return OBC_ERR_CODE_QUEUE_FULL;
}

obc_error_code_t sendToCDHEPSRespTxQueue(cdh_eps_resp_msg_t *resp) {
    if(cdhepsRespTxQueueHandle == NULL) {
        return OBC_ERR_CODE_INVALID_STATE;
    }

    if(resp == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    if(xQueueSend(cdhepsRespTxQueueHandle, (void *) resp, portMAX_DELAY) == pdPASS) {
        return OBC_ERR_CODE_SUCCESS;
    }

    return OBC_ERR_CODE_QUEUE_FULL;    
}

obc_error_code_t sendToCDHEPSRespRxQueue(cdh_eps_resp_msg_t *resp) {
    if(cdhepsRespRxQueueHandle == NULL) {
        return OBC_ERR_CODE_INVALID_STATE;
    }

    if(resp == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    if(xQueueSend(cdhepsRespRxQueueHandle, (void *) resp, portMAX_DELAY) == pdPASS) {
        return OBC_ERR_CODE_SUCCESS;
    }

    return OBC_ERR_CODE_QUEUE_FULL;    
}

static void populateCmdTxData(uint8_t *txData, cdh_eps_cmd_msg_t *cmd) {
    txData[0] = cmd->id;
    for(uint8_t i = 1; i < 8; i++) {
        txData[i] = cmd->param[i-1];
    }
}

static void populateRespTxData(uint8_t *txData, cdh_eps_resp_msg_t *resp) {
    txData[0] = resp->id;
    for(uint8_t i = 1; i < 7; i++) {
        txData[i] = resp->reserved[i-1];
    }
    txData[7] = resp->request;
}

/* Process queued messages to transmit to EPS*/
static void processTxMessages(void) {
    cdh_eps_cmd_msg_t cmd;
    cdh_eps_resp_msg_t resp;

    if(xQueueReceive(cdhepsCmdTxQueueHandle, &cmd, portMAX_DELAY) == pdPASS) {
        LOG_DEBUG("Command for EPS %u", cmd.id);
        uint32_t txMessageBox; 

        switch (cmd.id) {
            case CMD_SUBSYS_SHUTDDOWN: /* Subsystem shutdown command */
                txMessageBox = canMESSAGE_BOX1;
                break;
            case CMD_HEARTBEAT: /* Heartbeat command */
                txMessageBox = canMESSAGE_BOX3;
                break;
            case CMD_GET_TELEMETRY: /* Get telemetry command */
                txMessageBox = canMESSAGE_BOX4;
                break;
            default:
                // Invalid cmd id recieved -> Don't send to EPS
                break;
        }

        uint8_t txData[8]; /* 8 byte tx data buffer to send to EPS over CAN */
        populateCmdTxData(txData, &cmd);

        canSendMessage(canREG1, txMessageBox, txData); /* Send command to eps over CAN */
    }

    if(xQueueReceive(cdhepsRespTxQueueHandle, &resp, portMAX_DELAY) == pdPASS) {
        LOG_DEBUG("Response for EPS %u", resp.id);
        uint32_t txMessageBox;

        switch (resp.id) {
            case RESP_SUBSYS_SHUTDDOWN_ACK: /* Subsystem shutdown ACK */
                txMessageBox = canMESSAGE_BOX6;
                break;
            case RESP_HEARTBEAT_ACK:
                txMessageBox = canMESSAGE_BOX7;
                break;
            default:
                // Invalid resp id -> Don't send to EPS
                break;
        }

        uint8_t txData[8]; /* 8 byte tx data buffer to send to EPS over CAN */
        populateRespTxData(txData, &resp);

        canSendMessage(canREG1, txMessageBox, txData); /* Send command to eps over CAN */
    }
}

/* Process recieved messages from EPS */
static void processRxMessages(void) {
    cdh_eps_cmd_msg_t cmd;
    cdh_eps_resp_msg_t resp;
    /* Process recieved messages from EPS */
    if(xQueueReceive(cdhepsCmdRxQueueHandle, &cmd, portMAX_DELAY) == pdPASS) {
        switch (cmd.id) {
            case CMD_SUBSYS_SHUTDDOWN:
                break;
            case CMD_HEARTBEAT:
                break;
            case CMD_GET_TELEMETRY:
                break;
            default:
                break;
        }
    }

    if(xQueueReceive(cdhepsRespRxQueueHandle, &resp, portMAX_DELAY) == pdPASS) {
        switch (resp.id) {
            case RESP_SUBSYS_SHUTDDOWN_ACK:
                break;
            case RESP_HEARTBEAT_ACK:
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