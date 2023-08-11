#include "comms_manager.h"
#include "uplink_decoder.h"
#include "downlink_encoder.h"
#include "obc_gs_aes128.h"
#include "obc_gs_fec.h"
#include "obc_errors.h"
#include "obc_logging.h"
#include "obc_task_config.h"
#include "telemetry_manager.h"
#include "telemetry_fs_utils.h"
#include "obc_gs_telemetry_pack.h"
#include "obc_reliance_fs.h"
#include "telemetry_manager.h"
#include "cc1120_txrx.h"
#include "obc_privilege.h"

#if COMMS_PHY == COMMS_PHY_UART
#include "obc_sci_io.h"
#endif

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_queue.h>
#include <os_task.h>

#include <redposix.h>

#include <sys_common.h>
#include <gio.h>

#define COMMS_MAX_DOWNLINK_FRAMES 1000U

/* Comms Manager event queue config */
#define COMMS_MANAGER_QUEUE_LENGTH 10U
#define COMMS_MANAGER_QUEUE_ITEM_SIZE sizeof(comms_event_t)
#define COMMS_MANAGER_QUEUE_RX_WAIT_PERIOD pdMS_TO_TICKS(10)
#define COMMS_MANAGER_QUEUE_TX_WAIT_PERIOD pdMS_TO_TICKS(10)

static TaskHandle_t commsTaskHandle = NULL;
static StaticTask_t commsTaskBuffer;
static StackType_t commsTaskStack[COMMS_MANAGER_STACK_SIZE];

static QueueHandle_t commsQueueHandle = NULL;
static StaticQueue_t commsQueue;
static uint8_t commsQueueStack[COMMS_MANAGER_QUEUE_LENGTH * COMMS_MANAGER_QUEUE_ITEM_SIZE];

#define CC1120_TRANSMIT_QUEUE_LENGTH 3U
#define CC1120_TRANSMIT_QUEUE_ITEM_SIZE sizeof(transmit_event_t)
#define CC1120_TRANSMIT_QUEUE_RX_WAIT_PERIOD portMAX_DELAY
#define CC1120_TRANSMIT_QUEUE_TX_WAIT_PERIOD portMAX_DELAY
#define CC1120_SYNC_EVENT_SEMAPHORE_TIMEOUT pdMS_TO_TICKS(30000)
#define CC1120_TX_FIFO_EMPTY_SEMAPHORE_TIMEOUT pdMS_TO_TICKS(5000)

#define UART_MUTEX_BLOCK_TIME portMAX_DELAY

static QueueHandle_t cc1120TransmitQueueHandle = NULL;
static StaticQueue_t cc1120TransmitQueue;
static uint8_t cc1120TransmitQueueStack[CC1120_TRANSMIT_QUEUE_LENGTH * CC1120_TRANSMIT_QUEUE_ITEM_SIZE];

static const uint8_t TEMP_STATIC_KEY[AES_KEY_SIZE] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                                      0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};

/**
 * @brief	Comms Manager task.
 * @param	pvParameters	Task parameters.
 */
static void vCommsManagerTask(void *pvParameters);

void initCommsManager(void) {
  ASSERT((commsTaskStack != NULL) && (&commsTaskBuffer != NULL));
  if (commsTaskHandle == NULL) {
    commsTaskHandle = xTaskCreateStatic(vCommsManagerTask, COMMS_MANAGER_NAME, COMMS_MANAGER_STACK_SIZE, NULL,
                                        COMMS_MANAGER_PRIORITY, commsTaskStack, &commsTaskBuffer);
  }

  ASSERT((commsQueueStack != NULL) && (&commsQueue != NULL));
  if (commsQueueHandle == NULL) {
    commsQueueHandle =
        xQueueCreateStatic(COMMS_MANAGER_QUEUE_LENGTH, COMMS_MANAGER_QUEUE_ITEM_SIZE, commsQueueStack, &commsQueue);
  }

  ASSERT((cc1120TransmitQueueStack != NULL) && (&cc1120TransmitQueue != NULL))
  if (cc1120TransmitQueueHandle == NULL) {
    cc1120TransmitQueueHandle = xQueueCreateStatic(CC1120_TRANSMIT_QUEUE_LENGTH, CC1120_TRANSMIT_QUEUE_ITEM_SIZE,
                                                   cc1120TransmitQueueStack, &cc1120TransmitQueue);
  }

  // TODO: Implement a key exchange algorithm instead of using Pre-Shared/static key
  initializeAesCtx(TEMP_STATIC_KEY);
  initRs();

  initDecodeTask();

  initTelemEncodeTask();
}

obc_error_code_t sendToCommsManagerQueue(comms_event_t *event) {
  ASSERT(commsQueueHandle != NULL);

  if (event == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  if (xQueueSend(commsQueueHandle, (void *)event, COMMS_MANAGER_QUEUE_TX_WAIT_PERIOD) == pdPASS) {
    return OBC_ERR_CODE_SUCCESS;
  }

  return OBC_ERR_CODE_QUEUE_FULL;
}

obc_error_code_t sendToFrontCommsManagerQueue(comms_event_t *event) {
  ASSERT(commsQueueHandle != NULL);

  if (event == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  if (xQueueSendToFront(commsQueueHandle, (void *)event, COMMS_MANAGER_QUEUE_TX_WAIT_PERIOD) == pdPASS) {
    return OBC_ERR_CODE_SUCCESS;
  }
  return OBC_ERR_CODE_QUEUE_FULL;
}

static void vCommsManagerTask(void *pvParameters) {
  obc_error_code_t errCode;

  while (1) {
    comms_event_t queueMsg;

    if (xQueueReceive(commsQueueHandle, &queueMsg, COMMS_MANAGER_QUEUE_RX_WAIT_PERIOD) != pdPASS) {
      continue;
    }

    switch (queueMsg.eventID) {
      case BEGIN_DOWNLINK:
        for (uint16_t i = 0; i < COMMS_MAX_DOWNLINK_FRAMES; ++i) {
          transmit_event_t transmitEvent;
          // poll the transmit queue
          if (xQueueReceive(cc1120TransmitQueueHandle, &transmitEvent, CC1120_TRANSMIT_QUEUE_RX_WAIT_PERIOD) !=
              pdPASS) {
            LOG_ERROR_CODE(OBC_ERR_CODE_QUEUE_EMPTY);
          }
          if (transmitEvent.eventID == DOWNLINK_PACKET) {
#if COMMS_PHY == COMMS_PHY_UART
            LOG_IF_ERROR_CODE(sciSendBytes((uint8_t *)transmitEvent.ax25Pkt.data, transmitEvent.ax25Pkt.length,
                                           UART_MUTEX_BLOCK_TIME));
#else
            LOG_IF_ERROR_CODE(cc1120Send((uint8_t *)transmitEvent.ax25Pkt.data, transmitEvent.ax25Pkt.length,
                                         CC1120_TX_FIFO_EMPTY_SEMAPHORE_TIMEOUT));
#endif
          } else if (transmitEvent.eventID == END_DOWNLINK) {
            break;
          } else {
            LOG_ERROR_CODE(OBC_ERR_CODE_INVALID_ARG);
          }
        }
        break;
      case BEGIN_UPLINK:
#if COMMS_PHY == COMMS_PHY_UART
        uint8_t rxByte;

        // Read first byte
        LOG_IF_ERROR_CODE(sciReadBytes(&rxByte, 1, portMAX_DELAY, pdMS_TO_TICKS(1000)));

        if (errCode == OBC_ERR_CODE_SUCCESS) {
          LOG_IF_ERROR_CODE(sendToDecodeDataQueue(&rxByte));
        }

        if (errCode == OBC_ERR_CODE_SUCCESS) {
          // Read the rest of the bytes until we stop uplinking
          for (uint16_t i = 0; i < AX25_MAXIMUM_PKT_LEN; ++i) {
            LOG_IF_ERROR_CODE(sciReadBytes(&rxByte, 1, portMAX_DELAY, pdMS_TO_TICKS(10)));

            if (errCode == OBC_ERR_CODE_SUCCESS) {
              LOG_IF_ERROR_CODE(sendToDecodeDataQueue(&rxByte));
            }
          }
        }
#if CSDC_DEMO_ENABLED == 1
        comms_event_t event = {0};
        event.eventID = BEGIN_UPLINK;
        LOG_IF_ERROR_CODE(sendToCommsManagerQueue(&event));
#endif
#else
        // switch cc1120 to receive mode and start receiving all the bytes for one continuous transmission
        LOG_IF_ERROR_CODE(cc1120Receive(CC1120_SYNC_EVENT_SEMAPHORE_TIMEOUT));
        LOG_IF_ERROR_CODE(cc1120StrobeSpi(CC1120_STROBE_SFSTXON));
#endif
        break;
      default:
        LOG_ERROR_CODE(OBC_ERR_CODE_INVALID_ARG);
    }
  }
}

/**
 * @brief Sends an AX.25 packet to the CC1120 transmit queue
 *
 * @param ax25Pkt - Pointer to the AX.25 packet to send
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if the packet was sent to the queue
 */
obc_error_code_t sendToCC1120TransmitQueue(transmit_event_t *event) {
  ASSERT(cc1120TransmitQueueHandle != NULL);

  if (event == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  if (xQueueSend(cc1120TransmitQueueHandle, (void *)event, CC1120_TRANSMIT_QUEUE_TX_WAIT_PERIOD) == pdPASS) {
    return OBC_ERR_CODE_SUCCESS;
  }

  return OBC_ERR_CODE_QUEUE_FULL;
}
