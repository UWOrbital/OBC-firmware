#include "fec.h"
#include "ax25.h"
#include "encode_telemetry.h"
#include "send_telemetry.h"
#include "cc1120_txrx.h"

#include "telemetry_fs_utils.h"
#include "telemetry_manager.h"
#include "telemetry_pack.h"

#include "obc_task_config.h"
#include "obc_logging.h"
#include "obc_errors.h"
#include "obc_fs_utils.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_queue.h>
#include <os_task.h>

#include <sys_common.h>
#include <gio.h>

static TaskHandle_t telemEncodeTaskHandle = NULL;
static StaticTask_t telemEncodeTaskBuffer;
static StackType_t telemEncodeTaskStack[COMMS_TELEM_ENCODE_STACK_SIZE];

#define COMMS_TELEM_ENCODE_QUEUE_LENGTH 10U
#define COMMS_TELEM_ENCODE_QUEUE_ITEM_SIZE sizeof(uint32_t) // Size of the telemetry batch ID
#define COMMS_TELEM_ENCODE_QUEUE_RX_WAIT_PERIOD portMAX_DELAY
#define COMMS_TELEM_ENCODE_QUEUE_TX_WAIT_PERIOD portMAX_DELAY

static QueueHandle_t telemEncodeQueueHandle = NULL;
static StaticQueue_t telemEncodeQueue;
static uint8_t telemEncodeQueueStack[COMMS_TELEM_ENCODE_QUEUE_LENGTH*COMMS_TELEM_ENCODE_QUEUE_ITEM_SIZE];

/**
 * @brief Puts telemetry data through OSI model layers and queues into the CC1120 transmit queue
 * 
 * @param pvParameters - NULL
 */
static void vTelemEncodeTask(void *pvParameters);

/**
 * @brief Reads chunks of telemetry out of a file and sends it into the CC1120 transmit queue
 * 
 * @param telemetryBatchId - ID of the telemetry batch to send
 * @return obc_error_code_t - OBC_ERR_CODE_SUCCESS if all telemetry data was sent successfully
 */
static obc_error_code_t sendTelemetry(int32_t telemetryBatchId);

/**
 * @brief Initializes the telemetry encoding task and queue
 * 
 */
void initTelemEncodeTask(void) {
    ASSERT( (telemEncodeTaskStack != NULL) && (&telemEncodeTaskBuffer != NULL) );

    if (telemEncodeTaskHandle == NULL) {
        telemEncodeTaskHandle = xTaskCreateStatic(vTelemEncodeTask, COMMS_TELEM_ENCODE_TASK_NAME, COMMS_TELEM_ENCODE_STACK_SIZE,
                                                  NULL, COMMS_TELEM_ENCODE_TASK_PRIORITY, telemEncodeTaskStack,
                                                  &telemEncodeTaskBuffer);
    }

    if (telemEncodeQueueHandle == NULL) {
        telemEncodeQueueHandle = xQueueCreateStatic(COMMS_TELEM_ENCODE_QUEUE_LENGTH, COMMS_TELEM_ENCODE_QUEUE_ITEM_SIZE,
                                                    telemEncodeQueueStack, &telemEncodeQueue);
    }
}

/**
 * @brief Sends a telemetry batch ID to the telemetry encoding task to begin downlinking data
 * 
 * @param telemetryBatchId - ID of the telemetry batch to send
 * @return obc_error_code_t - OBC_ERR_CODE_SUCCESS if the telemetry batch ID was successfully sent to the queue
 */
obc_error_code_t sendToTelemEncodeQueue(uint32_t telemetryBatchId) {
    ASSERT(telemEncodeQueueHandle != NULL);
        
    if (xQueueSend(telemEncodeQueueHandle, (void *) &telemetryBatchId, COMMS_TELEM_ENCODE_QUEUE_TX_WAIT_PERIOD) == pdPASS) {
        return OBC_ERR_CODE_SUCCESS;
    }
    
    return OBC_ERR_CODE_QUEUE_FULL;
}

/**
 * @brief Puts telemetry data through OSI model layers and queues into the CC1120 transmit queue
 * 
 * @param pvParameters - NULL
 */
static void vTelemEncodeTask(void *pvParameters) {
    obc_error_code_t errCode;

    while (1) {
        uint32_t telemetryBatchId;

        // Wait for a telemetry downlink event
        if (xQueueReceive(telemEncodeQueueHandle, &telemetryBatchId, COMMS_TELEM_ENCODE_QUEUE_RX_WAIT_PERIOD) != pdPASS) {
            // TODO: Handle this if necessary
            continue;
        }

        // Open the telemetry file
        int32_t fd;
        errCode = openTelemetryFileRO(telemetryBatchId, &fd);
        if (errCode != OBC_ERR_CODE_SUCCESS) {
            LOG_ERROR_CODE(errCode);
            continue;
        }

        // Print the telemetry file size for debugging
        size_t fileSize;
        errCode = getFileSize(fd, &fileSize);
        if (errCode != OBC_ERR_CODE_SUCCESS) {
            LOG_ERROR_CODE(errCode);
            continue;
        }
        LOG_DEBUG("Sending telemetry file with size: %lu", fileSize);

        // Print telemetry file name
        char fileName[TELEMETRY_FILE_PATH_MAX_LENGTH] = {0};
        errCode = constructTelemetryFilePath(telemetryBatchId, fileName, TELEMETRY_FILE_PATH_MAX_LENGTH);
        if (errCode != OBC_ERR_CODE_SUCCESS) {
            LOG_ERROR_CODE(errCode);
            continue;
        }
        LOG_DEBUG("Sending telemetry file with name: %s", fileName);

        // Send the telemetry
        errCode = sendTelemetry(fd);
        if (errCode != OBC_ERR_CODE_SUCCESS) {
            LOG_ERROR_CODE(errCode);
            continue;
        }

        // Close telemetry file
        LOG_IF_ERROR_CODE(closeTelemetryFile(fd));
    }
}

/**
 * @brief Reads chunks of telemetry out of a file and sends it into the CC1120 transmit queue
 * 
 * @param fd - File descriptor of the telemetry file to send
 * @return obc_error_code_t - OBC_ERR_CODE_SUCCESS if all telemetry data was sent successfully
 */
static obc_error_code_t sendTelemetry(int32_t fd) {
    obc_error_code_t errCode;
    
    // Initialize important variables related to packing and queueing the telemetry to be sent
    telemetry_data_t singleTelem; // Holds a single piece of telemetry from getNextTelemetry()
    uint8_t packedSingleTelem[MAX_TELEMETRY_DATA_SIZE]; // Holds a serialized version of the current piece of telemetry

    packed_telem_packet_t telemPacket = {0}; // Holds 223B of "raw" telemetry data.
                                             // Zero initialized because telem IDs of 0 are ignored at the ground station
    size_t telemPacketOffset = 0; // Number of bytes filled in telemPacket

    packed_rs_packet_t fecPkt; // Holds a 255B RS packet
    packed_ax25_packet_t ax25Pkt; // Holds an AX.25 packet

    // Read a single piece of telemetry from the file
    while ((errCode = readNextTelemetryFromFile(fd, &singleTelem)) == OBC_ERR_CODE_SUCCESS) {
        LOG_DEBUG("Sending telemetry: %u", singleTelem.id);

        size_t packedSingleTelemSize = 0; // Size of the packed single telemetry
        // Pack the single telemetry into a uint8_t array
        RETURN_IF_ERROR_CODE(packTelemetry(&singleTelem,
                             packedSingleTelem,
                             sizeof(packedSingleTelem)/sizeof(uint8_t),
                             &packedSingleTelemSize));
        
        // If the single telemetry is too large to continue adding to the telemPacket, send the telemPacket
        if (telemPacketOffset + packedSingleTelemSize > PACKED_TELEM_PACKET_SIZE) {
            // Apply Reed Solomon FEC
            RETURN_IF_ERROR_CODE(rsEncode(&telemPacket, &fecPkt));

            // Perform AX.25 framing
            RETURN_IF_ERROR_CODE(ax25Send(&fecPkt, &ax25Pkt));

            // Send into CC1120 transmit queue
            RETURN_IF_ERROR_CODE(sendToCC1120TransmitQueue(&ax25Pkt));

            // Reset the packedTelem struct and offset
            telemPacket = (packed_telem_packet_t){0};
            telemPacketOffset = 0;
        }

        // Copy the telemetry data into the packedTelem struct
        memcpy(&telemPacket.data[telemPacketOffset], packedSingleTelem, packedSingleTelemSize);
        telemPacketOffset += packedSingleTelemSize;
    }

    if (errCode == OBC_ERR_CODE_REACHED_EOF) {
        LOG_DEBUG("Reached end of telemetry file");
        errCode = OBC_ERR_CODE_SUCCESS;
    }
    
    RETURN_IF_ERROR_CODE(errCode); // If the error wasn't an EOF error, return

    // If there's no data left to send, return
    if (telemPacketOffset == 0)
        return OBC_ERR_CODE_SUCCESS;

    // If there is data left, send it
    // (any "unfilled" bytes in the telemPacket are ignored by the ground station as they have telem ID 0)
    RETURN_IF_ERROR_CODE(rsEncode(&telemPacket, &fecPkt));
    RETURN_IF_ERROR_CODE(ax25Send(&fecPkt, &ax25Pkt));
    RETURN_IF_ERROR_CODE(sendToCC1120TransmitQueue(&ax25Pkt));

    return OBC_ERR_CODE_SUCCESS;
}
