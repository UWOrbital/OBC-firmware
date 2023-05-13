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
    [CMD_SUBSYS_SHUTDDOWN] = canMESSAGE_BOX1,
    [CMD_HEARTBEAT] = canMESSAGE_BOX3,
    [CMD_GET_TELEMETRY] = canMESSAGE_BOX4,
    [RESP_SUBSYS_SHUTDDOWN_ACK] = canMESSAGE_BOX6,
    [RESP_HEARTBEAT_ACK] = canMESSAGE_BOX7};

static const rx_callback_t rxCallbacks[] = {
    [CMD_SUBSYS_SHUTDDOWN] = subsysShutdownCmdHandler,
    [CMD_HEARTBEAT] = heartbeatCmdHandler,
    [CMD_GET_TELEMETRY] = getTelemetryCmdHandler,
    [TLE_EPS_BOARD_TEMP] = tleMsgHandler,
    [TLE_SOLAR_PANEL_1_TEMP] = tleMsgHandler,
    [TLE_SOLAR_PANEL_2_TEMP] = tleMsgHandler,
    [TLE_SOLAR_PANEL_3_TEMP] = tleMsgHandler,
    [TLE_SOLAR_PANEL_4_TEMP] = tleMsgHandler,
    [TLE_COMMS_5V_CURRENT] = tleMsgHandler,
    [TLE_COMMS_3V3_CURRENT] = tleMsgHandler,
    [TLE_MAG_8V_CURRENT] = tleMsgHandler,
    [TLE_ADCS_5V_CURRENT] = tleMsgHandler,
    [TLE_ADCS_3V3_CURRENT] = tleMsgHandler,
    [TLE_OBC_CURRENT] = tleMsgHandler,
    [TLE_COMMS_5V_CURRENT] = tleMsgHandler,
    [TLE_COMMS_3V3_VOLTAGE] = tleMsgHandler,
    [TLE_MAG_8V_VOLTAGE] = tleMsgHandler,
    [TLE_ADCS_5V_VOLTAGE] = tleMsgHandler,
    [TLE_ADCS_3V3_VOLTAGE] = tleMsgHandler,
    [TLE_OBC_3V3_VOLTAGE] = tleMsgHandler,
    [RESP_SUBSYS_SHUTDDOWN_ACK] = respSubsysShutdownAckHandler,
    [RESP_HEARTBEAT_ACK] = respHeartbeatAckHandler};

static const size_t txMessageBoxesArraySize = sizeof(txMessageBoxes) / sizeof(uint8_t);
static const size_t rxCallbackArraySize = sizeof(rxCallbacks) / sizeof(rx_callback_t);
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

    if (xQueueReceive(cdhepsTxQueueHandle, &msg, portMAX_DELAY) == pdPASS)
    {
        uint32_t txMessageBox;

        // Check if the ID is a valid tx message 
        if (msg.cmd.id >= txMessageBoxesArraySize)
        {
            LOG_ERROR_CODE(OBC_ERR_CODE_UNSUPPORTED_CAN_MSG);
        }
        else
        {
            // Check if the ID has a messge box
            if (txMessageBoxes[msg.cmd.id] == 0)
            {
                LOG_ERROR_CODE(OBC_ERR_CODE_UNSUPPORTED_CAN_MSG);
            }
            else
            {
                txMessageBox = txMessageBoxes[msg.cmd.id];
            }
        }

        /* Send tx message to eps over CAN */
        if(canSendMessage(canREG1, txMessageBox, (uint8_t *)&msg) == OBC_ERR_CODE_CAN_FAILURE) {
            LOG_DEBUG("Failed to send CAN msg to eps with ID: %u", msg.cmd.id);
        }
        else {
            LOG_DEBUG("Sending CAN msg to eps with ID: %u", msg.cmd.id);
        }
    }
}

/* Process recieved messages from EPS */
static void processRxMessages(void)
{
    obc_error_code_t errCode;

    cdh_eps_queue_msg_t msg;
    /* Process recieved messages from EPS */
    if (xQueueReceive(cdhepsRxQueueHandle, &msg, portMAX_DELAY) == pdPASS)
    {
        LOG_DEBUG("Recieved CAN msg from EPS with ID: %u", msg.cmd.id);
        
        // Check if the ID is a valid rx message
        if (msg.cmd.id >= rxCallbackArraySize)
        {
            LOG_ERROR_CODE(OBC_ERR_CODE_UNSUPPORTED_CAN_MSG);
        }
        else
        {
            // Check if the ID has a callback, if it does, execute
            if (rxCallbacks[msg.cmd.id] == NULL)
            {
                LOG_ERROR_CODE(OBC_ERR_CODE_UNSUPPORTED_CAN_MSG);
            }
            else
            { // Execute callback
                LOG_IF_ERROR_CODE(rxCallbacks[msg.cmd.id](&msg));
                rxCallbacks[msg.cmd.id](&msg);
            }
        }
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