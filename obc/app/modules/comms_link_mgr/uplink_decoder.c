#include "uplink_decoder.h"
#include "obc_gs_aes128.h"
#include "obc_gs_ax25.h"
#include "obc_gs_fec.h"
#include "cc1120_txrx.h"
#include "cc1120_defs.h"
#include "cc1120.h"
#include "obc_gs_command_unpack.h"
#include "obc_gs_command_id.h"
#include "obc_gs_command_data.h"
#include "command_manager.h"
#include "obc_scheduler_config.h"
#include "obc_logging.h"
#include "comms_manager.h"
#include "uplink_flow.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_queue.h>
#include <os_task.h>
#include <os_semphr.h>
#include <sys_common.h>
#include <gio.h>
#include <os_timer.h>

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

// decode data queue length should be double TXRX_INTERRUPT_THRESHOLD for safety to avoid cc1120 getting blocked
// can be reduced later depending on memory limitations
#define DECODE_DATA_QUEUE_LENGTH 2 * TXRX_INTERRUPT_THRESHOLD
#define DECODE_DATA_QUEUE_ITEM_SIZE sizeof(uint8_t)
#define DECODE_DATA_QUEUE_RX_WAIT_PERIOD portMAX_DELAY
#define DECODE_DATA_QUEUE_TX_WAIT_PERIOD portMAX_DELAY
#define AX25_TIMEOUT_MILLISECONDS 330000
#define TIMER_QUEUE_TX_TIMEOUT_MILLISECONDS 500
#define TIMER_NAME "flag_timeout"

static bool isStartFlagReceived;

// Decode Data Queue
static QueueHandle_t decodeDataQueueHandle = NULL;
static StaticQueue_t decodeDataQueue;
static uint8_t decodeDataQueueStack[DECODE_DATA_QUEUE_LENGTH * DECODE_DATA_QUEUE_ITEM_SIZE];

static obc_error_code_t decodePacketAndSendCommand(packed_ax25_i_frame_t *ax25Data, packed_rs_packet_t *rsData,
                                                   aes_data_t *aesData);

/**
 * @brief parses the completely decoded data and sends it to the command manager and detects end of transmission
 *
 * @param cmdBytes 223B-AES_IV_SIZE array storing the completely decoded data
 * @param dataLen length of the data in cmdBytes
 *
 * @return obc_error_code_t - whether or not the data was successfullysent to the command manager
 */
obc_error_code_t handleCommands(uint8_t *cmdBytes) {
  obc_error_code_t errCode = OBC_ERR_CODE_SUCCESS;
  if (cmdBytes == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  uint32_t bytesUnpacked = 0;
  // Keep unpacking cmdBytes into cmd_msg_t commands to send to command manager until we have unpacked all the bytes in
  // cmdBytes If the command id is the id for end of transmission, isStillUplinking should be set to false
  while (bytesUnpacked < AES_DECRYPTED_SIZE) {
    if (cmdBytes[bytesUnpacked] == CMD_END_OF_FRAME) {
      // means we have reached the end of the packet and rest can be ignored
      return OBC_ERR_CODE_SUCCESS;
    }
    cmd_msg_t command;
    if (unpackCmdMsg(cmdBytes, &bytesUnpacked, &command) != OBC_GS_ERR_CODE_SUCCESS) {
      return OBC_ERR_CODE_FAILED_UNPACK;
    }

    RETURN_IF_ERROR_CODE(sendToCommandQueue(&command));
  }
  return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief flag timeout callback that sets isFlagReceived to false due to a timeout
 */
static void flagTimeoutCallback() { isStartFlagReceived = false; }

void obcTaskInitCommsUplinkDecoder(void) {
  ASSERT((decodeDataQueueStack != NULL) && (&decodeDataQueue != NULL));
  if (decodeDataQueueHandle == NULL) {
    decodeDataQueueHandle = xQueueCreateStatic(DECODE_DATA_QUEUE_LENGTH, DECODE_DATA_QUEUE_ITEM_SIZE,
                                               decodeDataQueueStack, &decodeDataQueue);
  }
}

void obcTaskFunctionCommsUplinkDecoder(void *pvParameters) {
  StaticTimer_t timerBuffer = {0};
  TimerHandle_t flagTimeoutTimer = xTimerCreateStatic(TIMER_NAME, pdMS_TO_TICKS(AX25_TIMEOUT_MILLISECONDS), pdFALSE,
                                                      (void *)0, flagTimeoutCallback, &timerBuffer);
  obc_error_code_t errCode;
  uint8_t byte = 0;

  packed_ax25_i_frame_t axData = {0};
  uint16_t axDataIndex = 0;

  bool startFlagReceived = false;

  while (1) {
    if (xQueueReceive(decodeDataQueueHandle, &byte, DECODE_DATA_QUEUE_RX_WAIT_PERIOD) == pdPASS) {
      if (axDataIndex >= sizeof(axData.data)) {
        LOG_ERROR_CODE(OBC_ERR_CODE_BUFF_OVERFLOW);

        // Restart the decoding process
        memset(&axData, 0, sizeof(axData));
        axDataIndex = 0;
        startFlagReceived = false;
      }

      if (byte == AX25_FLAG) {
        axData.data[axDataIndex++] = byte;

        // Decode packet if we have start flag, end flag, and at least 1 byte of data
        // During idling, multiple AX25_FLAGs may be sent in a row, so we enforce that
        // axData.data[1] must be something other than AX25_FLAG
        if (axDataIndex > 2) {
          axData.length = axDataIndex;

          packed_rs_packet_t rsData = {0};
          aes_data_t aesData = {0};
          LOG_IF_ERROR_CODE(decodePacketAndSendCommand(&axData, &rsData, &aesData));

          // Restart the decoding process
          memset(&axData, 0, sizeof(axData));
          axDataIndex = 0;
          axData.data[axDataIndex++] = AX25_FLAG;
        } else {
          if (!startFlagReceived) {
            if (xTimerStart(flagTimeoutTimer, pdMS_TO_TICKS(TIMER_QUEUE_TX_TIMEOUT_MILLISECONDS)) != pdPASS) {
              LOG_ERROR_CODE(OBC_ERR_CODE_QUEUE_FULL);
            }
          }
          startFlagReceived = true;
          axDataIndex = 1;
        }
        continue;
      }
      if (startFlagReceived) {
        axData.data[axDataIndex++] = byte;
      }
    }
  }
}

/**
 * @brief completely decode a received packet and send the command to the command manager
 *
 * @param data - packed ax25 packet with received data
 * @param rsData - holds packed reed solomon data
 * @param aesData - pointer to an aes_data_t type, which holds the data to decrypt & the IV
 * @param decryptedData - holds the decrypted data from the aesBlock
 *
 * @return obc_error_code_t - whether or not the data was completely decoded successfully
 */
static obc_error_code_t decodePacketAndSendCommand(packed_ax25_i_frame_t *ax25Data, packed_rs_packet_t *rsData,
                                                   aes_data_t *aesData) {
  obc_error_code_t errCode;
  uplink_flow_decoded_packet_t command = {0};
  RETURN_IF_ERROR_CODE(decodePacket(ax25Data, rsData, aesData, &command));

  // Handle the decoded data
  switch (command.type) {
    case UPLINK_FLOW_DECODED_CMD:
      RETURN_IF_ERROR_CODE(sendToCommsManagerQueue(&command.command));
      return OBC_ERR_CODE_SUCCESS;
    case UPLINK_FLOW_DECODED_DATA:
      RETURN_IF_ERROR_CODE(handleCommands(command.data));
      return OBC_ERR_CODE_SUCCESS;
  }
  return OBC_ERR_CODE_INVALID_STATE;
}

/**
 * @brief send a received packet to the decode data pipeline to be sent to command manager
 *
 * @param data pointer to a single byte
 *
 * @return obc_error_code_t - whether or not the packet was successfully sent to the queue
 */
obc_error_code_t sendToDecodeDataQueue(uint8_t *data) {
  if (decodeDataQueueHandle == NULL) {
    return OBC_ERR_CODE_INVALID_STATE;
  }

  if (data == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  if (xQueueSend(decodeDataQueueHandle, (void *)data, DECODE_DATA_QUEUE_TX_WAIT_PERIOD) == pdPASS) {
    return OBC_ERR_CODE_SUCCESS;
  }

  return OBC_ERR_CODE_QUEUE_FULL;
}
