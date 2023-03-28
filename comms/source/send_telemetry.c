#include "send_telemetry.h"
#include "ax25.h"
#include "fec.h"
#include "cc1120_txrx.h"
#include "telemetry_manager.h"
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

#define CC1120_TRANSMIT_QUEUE_LENGTH 10U
#define CC1120_TRANSMIT_QUEUE_ITEM_SIZE CC1120_TX_RX_PKT_SIZE
#define CC1120_TRANSMIT_QUEUE_RX_WAIT_PERIOD portMAX_DELAY

static QueueHandle_t cc1120TransmitQueueHandle = NULL;
static StaticQueue_t cc1120TransmitQueue = NULL;
static uint8_t cc1120TransmitQueueStack[CC1120_TRANSMIT_QUEUE_LENGTH*CC1120_TRANSMIT_QUEUE_ITEM_SIZE];

/**
 * @brief Packs an individual piece of telemetry into a packed_telem_t struct
 * 
 * @param singleTelem - Pointer to the telemetry data to pack
 * @param packedTelem - Pointer to the packed_telem_t struct to fill
 * @param residualData - Pointer to the packed_telem_t struct to fill with leftover data if residualData overflows
 * @param bytesFilled - Pointer to the total number of bytes in the packed_telem_t struct at the end of the function
 * @return obc_error_code_t - OBC_ERR_CODE_SUCCESS if successful, otherwise an error code
 */
obc_error_code_t packTelemetry(telemetry_data_t *singleTelem,
                               packed_telem_t *packedTelem,
                               packed_telem_t *residualData,
                               uint8_t *bytesFilled);

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
        RETURN_IF_NOT_ACQUIRED(xQueueReceive(cc1120TransmitQueueHandle, &ax25_pkt, CC1120_TRANSMIT_QUEUE_RX_WAIT_PERIOD));
        
        // Write to CC1120 FIFO
        cc1120Send((uint8_t *)ax25_pkt.data, AX25_PKT_LEN);
    }
}

/**
 * @brief Packs an individual piece of telemetry into a packed_telem_t struct
 * 
 * @param singleTelem - Pointer to the telemetry data to pack
 * @param packedTelem - Pointer to the packed_telem_t struct to fill
 * @param residualData - Pointer to the packed_telem_t struct to fill with leftover data if residualData overflows
 * @param bytesFilled - Pointer to the total number of bytes in the packed_telem_t struct at the end of the function
 * @return obc_error_code_t - OBC_ERR_CODE_SUCCESS if successful, otherwise an error code
 */
obc_error_code_t packTelemetry(telemetry_data_t *singleTelem,
                               packed_telem_t *packedTelem,
                               packed_telem_t *residualData,
                               uint8_t *bytesFilled) {
    obc_error_code_t errCode;
    
    if (singleTelem == NULL)
        return OBC_ERR_CODE_INVALID_ARG;
    
    if (packedTelem == NULL)
        return OBC_ERR_CODE_INVALID_ARG;
    
    if (residualData == NULL)
        return OBC_ERR_CODE_INVALID_ARG;
    
    if (bytesFilled == NULL)
        return OBC_ERR_CODE_INVALID_ARG;

    uint8_t sizeOfTelem = /* getTelemParamSize(singleTelem) + */ TELEM_TIMESTAMP_SIZE_BYTES + TELEM_ID_SIZE_BYTES;
    uint8_t totalBytesAfterWrite = *bytesFilled + sizeOfTelem;

    uint8_t serializedSingleParam[MAX_SINGLE_TELEM_SIZE_BYTES];
    memcpy(serializedSingleParam, singleTelem->id, TELEM_ID_SIZE_BYTES);
    memcpy(serializedSingleParam + TELEM_ID_SIZE_BYTES, singleTelem->timestamp, TELEM_TIMESTAMP_SIZE_BYTES);
    // memcpy(serializedSingleParam + TELEM_ID_SIZE_BYTES + TELEM_TIMESTAMP_SIZE_BYTES, getTelemParam(singleTelem), getTelemParamSize(singleTelem));

    if (totalBytesAfterWrite > PACKED_TELEM_PACKET_SIZE) {
        // Not enough space in packedTelem to fit singleTelem
        // Copy as much as possible into packedTelem
        uint8_t bytesToWrite = PACKED_TELEM_PACKET_SIZE - *bytesFilled;
        memcpy(packedTelem->data + *bytesFilled, serializedSingleParam, bytesToWrite);
        *bytesFilled += sizeOfTelem;
        // Copy remaining data into residualData
        memcpy(residualData->data, serializedSingleParam + bytesToWrite, totalBytesAfterWrite - PACKED_TELEM_PACKET_SIZE);
    } else {
        memcpy(packedTelem->data + *bytesFilled, serializedSingleParam, sizeOfTelem);
    }
    
    *bytesFilled = totalBytesAfterWrite;

    return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief Queues AX.25 packets into the CC1120 transmit queue
 * 
 * @param telemFileId - ID of the telemetry file to send
 */
void sendTelemetry(uint32_t telemFileId) {
    obc_error_code_t errCode;

    ASSERT (CC1120_TRANSMIT_QUEUE_ITEM_SIZE == AX25_PKT_LEN);

    // Open telemetry file
    // RETURN_IF_ERROR_CODE(openTelemetryFile(uint32_t telemFileId));
    
    telemetry_data_t singleTelem; // Holds a single piece of telemetry from getNextTelemetry()
    packed_telem_t packedTelem; // Holds 233B of telemetry
    packed_telem_t residualData; // Holds any residual telemetry information to write to a new RS packet
    uint8_t bytesFilled; // Number of bytes filled in packedTelem
    
    packed_rs_packet_t fecPkt; // Holds a 255B RS packet
    packed_ax25_packet_t ax25Pkt; // Holds an AX.25 packet

    // Read telemetry from file
    while ((errCode = getNextTelemetry(telemFileId, &singleTelem)) == OBC_ERR_CODE_SUCCESS) {
        LOG_DEBUG("Sending telemetry: %u", singleTelem.id);

        // Pack telemetry continuously into 233B packets for FEC
        packTelemetry(&singleTelem, &packedTelem, &residualData, &bytesFilled);
        if (bytesFilled > PACKED_TELEM_PACKET_SIZE) {
            // Apply Reed Solomon FEC
            RETURN_CLOSE_FILE_IF_ERROR_CODE(rsEncode(&packedTelem, &fecPkt));

            // Perform AX.25 framing
            RETURN_CLOSE_FILE_IF_ERROR_CODE(ax25Frame(&fecPkt, &ax25Pkt));

            // Send into CC1120 transmit queue
            if(xQueueSend(cc1120TransmitQueueHandle, &ax25Pkt, CC1120_TRANSMIT_QUEUE_RX_WAIT_PERIOD) != pdPASS) {
                LOG_ERROR("Failed to send telemetry into CC1120 transmit queue");
                errCode = OBC_ERR_CODE_QUEUE_FULL;
                return errCode;
            }
            
            // Move residual data to packedTelem
            packedTelem = residualData;
            bytesFilled = bytesFilled % PACKED_TELEM_PACKET_SIZE;
            residualData = (packed_telem_t){0};
        }
    }

    if (errCode == OBC_ERR_CODE_REACHED_EOF) {
        LOG_DEBUG("Reached end of telemetry file");
        errCode = OBC_ERR_CODE_SUCCESS;
    }

    RETURN_CLOSE_FILE_IF_ERROR_CODE(errCode);

    // Remaining data in packedTelem is 0s
    // Residual data should be "empty"
    RETURN_CLOSE_FILE_IF_ERROR_CODE(rsEncode(&packedTelem, &fecPkt));
    RETURN_CLOSE_FILE_IF_ERROR_CODE(ax25Frame(&fecPkt, &ax25Pkt));
    if(xQueueSend(cc1120TransmitQueueHandle, &ax25Pkt, CC1120_TRANSMIT_QUEUE_RX_WAIT_PERIOD) != pdPASS) {
        LOG_ERROR("Failed to send telemetry into CC1120 transmit queue");
        errCode = OBC_ERR_CODE_QUEUE_FULL;
        return errCode;
    }
    
    // Close telemetry file
    // closeTelemetryFile(telemFileId);
    
}
