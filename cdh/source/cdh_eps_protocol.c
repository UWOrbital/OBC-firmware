#include "cdh_eps_protocol.h"
#include "cdh_eps_protocol_id.h"
#include "cdh_eps_handlers.h"
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
static uint8_t cdhepsTxQueueStack[CDH_EPS_QUEUE_LENGTH * CDH_EPS_QUEUE_ITEM_SIZE];

static QueueHandle_t cdhepsRxQueueHandle;
static StaticQueue_t cdhepsRxQueue;
static uint8_t cdhepsRxQueueStack[CDH_EPS_QUEUE_LENGTH * CDH_EPS_QUEUE_ITEM_SIZE];

static const uint32_t txMessageBoxes[] = {
    [CMD_SUBSYS_SHUTDDOWN]      = canMESSAGE_BOX1,
    [CMD_HEARTBEAT]             = canMESSAGE_BOX3,
    [CMD_GET_TELEMETRY]         = canMESSAGE_BOX4,
    [TLE_EPS_BOARD_TEMP]        = 0,
    [TLE_SOLAR_PANEL_1_TEMP]    = 0,
    [TLE_SOLAR_PANEL_2_TEMP]    = 0,
    [TLE_SOLAR_PANEL_3_TEMP]    = 0,
    [TLE_SOLAR_PANEL_4_TEMP]    = 0,
    [TLE_COMMS_5V_CURRENT]      = 0,
    [TLE_COMMS_3V3_CURRENT]     = 0,
    [TLE_MAG_8V_CURRENT]        = 0,
    [TLE_ADCS_5V_CURRENT]       = 0,
    [TLE_ADCS_3V3_CURRENT]      = 0,
    [TLE_OBC_CURRENT]           = 0,
    [TLE_COMMS_5V_CURRENT]      = 0,
    [TLE_COMMS_3V3_VOLTAGE]     = 0,
    [TLE_MAG_8V_VOLTAGE]        = 0,
    [TLE_ADCS_5V_VOLTAGE]       = 0,
    [TLE_ADCS_3V3_VOLTAGE]      = 0,
    [TLE_OBC_3V3_VOLTAGE]       = 0,
    [RESP_SUBSYS_SHUTDDOWN_ACK] = canMESSAGE_BOX6,
    [RESP_HEARTBEAT_ACK]        = canMESSAGE_BOX7
};

#define TX_MSG_BOX_SIZE (sizeof(txMessageBoxes) / sizeof(uint32_t))

static const rx_handler_t rxHandlers[] = {
    [CMD_SUBSYS_SHUTDDOWN]      = subsysShutdownCmdHandler,
    [CMD_HEARTBEAT]             = NULL,
    [CMD_GET_TELEMETRY]         = NULL,
    [TLE_EPS_BOARD_TEMP]        = tleMsgHandler,
    [TLE_SOLAR_PANEL_1_TEMP]    = tleMsgHandler,
    [TLE_SOLAR_PANEL_2_TEMP]    = tleMsgHandler,
    [TLE_SOLAR_PANEL_3_TEMP]    = tleMsgHandler,
    [TLE_SOLAR_PANEL_4_TEMP]    = tleMsgHandler,
    [TLE_COMMS_5V_CURRENT]      = tleMsgHandler,
    [TLE_COMMS_3V3_CURRENT]     = tleMsgHandler,
    [TLE_MAG_8V_CURRENT]        = tleMsgHandler,
    [TLE_ADCS_5V_CURRENT]       = tleMsgHandler,
    [TLE_ADCS_3V3_CURRENT]      = tleMsgHandler,
    [TLE_OBC_CURRENT]           = tleMsgHandler,
    [TLE_COMMS_5V_CURRENT]      = tleMsgHandler,
    [TLE_COMMS_3V3_VOLTAGE]     = tleMsgHandler,
    [TLE_MAG_8V_VOLTAGE]        = tleMsgHandler,
    [TLE_ADCS_5V_VOLTAGE]       = tleMsgHandler,
    [TLE_ADCS_3V3_VOLTAGE]      = tleMsgHandler,
    [TLE_OBC_3V3_VOLTAGE]       = tleMsgHandler,
    [RESP_SUBSYS_SHUTDDOWN_ACK] = respSubsysShutdownAckHandler,
    [RESP_HEARTBEAT_ACK]        = respHeartbeatAckHandler
};

#define RX_HANDLERS_SIZE (sizeof(rxHandlers) / sizeof(rx_handler_t))

/**
 * @brief Task that manages the CAN TX and RX between CDH and EPS
 */
static void cdhepsProtocolTask(void *pvParameters);

void initCDHEPSProtocol(void)
{
    ASSERT((cdhepsProtocolStack != NULL) && (&cdhepsProtocolTaskBuffer != NULL));
    if (cdhepsProtocolTaskHandle == NULL)
    {
        cdhepsProtocolTaskHandle = xTaskCreateStatic(
            cdhepsProtocolTask,
            CDH_EPS_NAME,
            CDH_EPS_STACK_SIZE,
            NULL,
            CDH_EPS_PRIORITY,
            cdhepsProtocolStack,
            &cdhepsProtocolTaskBuffer);
    }

    ASSERT((cdhepsTxQueueStack != NULL) && (&cdhepsTxQueue != NULL));
    if (cdhepsTxQueueHandle == NULL)
    {
        cdhepsTxQueueHandle = xQueueCreateStatic(
            CDH_EPS_QUEUE_LENGTH,
            CDH_EPS_QUEUE_ITEM_SIZE,
            cdhepsTxQueueStack,
            &cdhepsTxQueue);
    }

    ASSERT((cdhepsRxQueueStack != NULL) && (&cdhepsRxQueue != NULL));
    if (cdhepsRxQueueHandle == NULL)
    {
        cdhepsRxQueueHandle = xQueueCreateStatic(
            CDH_EPS_QUEUE_LENGTH,
            CDH_EPS_QUEUE_ITEM_SIZE,
            cdhepsRxQueueStack,
            &cdhepsRxQueue);
    }
}

obc_error_code_t sendToCDHEPSTxQueue(cdh_eps_queue_msg_t *msg)
{
    if (msg == NULL)
    {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    if (cdhepsTxQueueHandle == NULL)
    {
        return OBC_ERR_CODE_INVALID_STATE;
    }

    if (xQueueSend(cdhepsTxQueueHandle, (void *)msg, portMAX_DELAY) == pdPASS)
    {
        return OBC_ERR_CODE_SUCCESS;
    }

    return OBC_ERR_CODE_QUEUE_FULL;
}

obc_error_code_t sendToCDHEPSRxQueue(cdh_eps_queue_msg_t *msg)
{
    if (msg == NULL)
    {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    if (cdhepsRxQueueHandle == NULL)
    {
        return OBC_ERR_CODE_INVALID_STATE;
    }

    if (xQueueSend(cdhepsRxQueueHandle, (void *)msg, portMAX_DELAY) == pdPASS)
    {
        return OBC_ERR_CODE_SUCCESS;
    }

    return OBC_ERR_CODE_QUEUE_FULL;
}

/* Process queued messages to transmit to EPS*/
static void processTxMessages(void)
{
    cdh_eps_queue_msg_t msg;

    if (xQueueReceive(cdhepsTxQueueHandle, &msg, 0) == pdPASS)
    {
        LOG_DEBUG("Message to transmit to EPS with ID: %u", msg.cmd.id);
        uint32_t txMessageBox;

        // Check if the ID has a messge box
        if(msg.cmd.id >= TX_MSG_BOX_SIZE) {
            LOG_ERROR_CODE(OBC_ERR_CODE_UNSUPPORTED_CMD);
            return;
        }

        if(!txMessageBoxes[msg.cmd.id]) {
            LOG_ERROR_CODE(OBC_ERR_CODE_UNSUPPORTED_CAN_MSG);
            return;
        }
        
        txMessageBox = txMessageBoxes[msg.cmd.id];

        /* Send tx message to eps over CAN */
        if(canSendMessage(canREG1, txMessageBox, (uint8_t *)&msg) == OBC_ERR_CODE_CAN_FAILURE) {
            LOG_DEBUG("Failed to send CAN msg to eps with ID: %u", msg.cmd.id);
            LOG_ERROR_CODE(OBC_ERR_CODE_CAN_FAILURE);
        }
        else {
            LOG_DEBUG("Sending CAN msg to eps with ID: %u", msg.cmd.id);
        }
    }
}

/* Process recieved messages from EPS */
static void processRxMessages(void)
{
    cdh_eps_queue_msg_t msg;
    /* Process recieved messages from EPS */
    if (xQueueReceive(cdhepsRxQueueHandle, &msg, 0) == pdPASS)
    {
        LOG_DEBUG("Recieved CAN msg from EPS with ID: %u", msg.cmd.id);
        if(msg.cmd.id >= RX_HANDLERS_SIZE) {
            LOG_ERROR_CODE(OBC_ERR_CODE_UNSUPPORTED_CMD);
            return;
        }

        // Check if the ID has a handler, if it does, execute
        if (rxHandlers[msg.cmd.id] == NULL)
        {
            LOG_ERROR_CODE(OBC_ERR_CODE_UNSUPPORTED_CAN_MSG);
            return;
        }

        // Execute callback
        obc_error_code_t errCode = rxHandlers[msg.cmd.id](&msg);
        LOG_IF_ERROR_CODE(errCode);
    }
}

static void cdhepsProtocolTask(void *pvParameters)
{
    while (1)
    {
        processTxMessages();
        processRxMessages();
    }
}