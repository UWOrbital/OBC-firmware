#include "send_telemetry.h"
#include "telemetry_manager.h"
#include "cc1120_txrx.h"
#include "obc_task_config.h"
#include "obc_logging.h"
#include "obc_errors.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_queue.h>
#include <os_task.h>

#include <sys_common.h>
#include <gio.h>

static TaskHandle_t cc1120TransmitTaskHandle = NULL;
static StaticTask_t cc1120TransmitTaskBuffer;
static StackType_t cc1120TransmitTaskStack[CC1120_TRANSMIT_STACK_SIZE];

#define CC1120_TRANSMIT_QUEUE_LENGTH 3U
#define CC1120_TRANSMIT_QUEUE_ITEM_SIZE AX25_PKT_LEN
#define CC1120_TRANSMIT_QUEUE_RX_WAIT_PERIOD portMAX_DELAY
#define CC1120_TRANSMIT_QUEUE_TX_WAIT_PERIOD portMAX_DELAY

static QueueHandle_t cc1120TransmitQueueHandle = NULL;
static StaticQueue_t cc1120TransmitQueue;
static uint8_t cc1120TransmitQueueStack[CC1120_TRANSMIT_QUEUE_LENGTH*CC1120_TRANSMIT_QUEUE_ITEM_SIZE];

/**
 * @brief Sends data from the CC1120 transmit queue into the CC1120 FIFO memory
 * 
 * @param pvParameters - NULL
 */
static void vCC1120TransmitTask(void *pvParameters);

/**
 * @brief Initialize the CC1120 transmit task and queue
 * 
 */
void initCC1120TransmitTask(void) {
    ASSERT( (cc1120TransmitTaskStack != NULL) && (&cc1120TransmitTaskBuffer != NULL) );

    if (cc1120TransmitTaskHandle == NULL) {
        cc1120TransmitTaskHandle = xTaskCreateStatic(vCC1120TransmitTask, CC1120_TRANSMIT_TASK_NAME, CC1120_TRANSMIT_STACK_SIZE,
                                                     NULL, CC1120_TRANSMIT_TASK_PRIORITY, cc1120TransmitTaskStack,
                                                     &cc1120TransmitTaskBuffer);
    }

    if (cc1120TransmitQueueHandle == NULL) {
        cc1120TransmitQueueHandle = xQueueCreateStatic(CC1120_TRANSMIT_QUEUE_LENGTH, CC1120_TRANSMIT_QUEUE_ITEM_SIZE,
                                                       cc1120TransmitQueueStack, &cc1120TransmitQueue);
    }
}

/**
 * @brief Sends an AX.25 packet to the CC1120 transmit queue
 * 
 * @param ax25Pkt - Pointer to the AX.25 packet to send
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if the packet was sent to the queue
 */
obc_error_code_t sendToCC1120TransmitQueue(packed_ax25_packet_t *ax25Pkt) {
    ASSERT(cc1120TransmitQueueHandle != NULL);
    
    if (ax25Pkt == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }
    
    if (xQueueSend(cc1120TransmitQueueHandle, (void *) ax25Pkt, CC1120_TRANSMIT_QUEUE_TX_WAIT_PERIOD) == pdPASS) {
        return OBC_ERR_CODE_SUCCESS;
    }
    
    return OBC_ERR_CODE_QUEUE_FULL;
}

/**
 * @brief Sends data from the CC1120 transmit queue into the CC1120 FIFO memory
 * 
 * @param pvParameters - NULL
 */
static void vCC1120TransmitTask(void *pvParameters) {
    ASSERT( (cc1120TransmitQueueHandle != NULL) );
    obc_error_code_t errCode;

    while (1) {
        // Wait for CC1120 transmit queue
        packed_ax25_packet_t ax25_pkt;
        if (xQueueReceive(cc1120TransmitQueueHandle, &ax25_pkt, CC1120_TRANSMIT_QUEUE_RX_WAIT_PERIOD) != pdPASS) {
            // TODO: Handle this if necessary
            continue;
        }
        
        // Write to CC1120 FIFO
        LOG_IF_ERROR_CODE(cc1120Send((uint8_t *)ax25_pkt.data, AX25_PKT_LEN));
    }
}
