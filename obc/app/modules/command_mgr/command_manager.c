#include "alarm_handler.h"
#include "command.h"
#include "command_manager.h"
#include "comms_manager.h"
#include "obc_gs_ax25.h"
#include "obc_gs_command_data.h"
#include "obc_errors.h"
#include "obc_gs_commands_response.h"
#include "obc_gs_commands_response_pack.h"
#include "obc_gs_errors.h"
#include "obc_logging.h"
#include "obc_gs_fec.h"
#include "obc_gs_commands_response_pack.h"

#include <FreeRTOS.h>
#include <sys_common.h>
#include <os_task.h>
#include <os_queue.h>

#define COMMAND_QUEUE_LENGTH 25UL
#define COMMAND_QUEUE_ITEM_SIZE sizeof(cmd_msg_t)

static QueueHandle_t commandQueueHandle;
static StaticQueue_t commandQueue;
static uint8_t commandQueueStack[COMMAND_QUEUE_LENGTH * COMMAND_QUEUE_ITEM_SIZE];

void obcTaskInitCommandMgr(void) {
  ASSERT((commandQueueStack != NULL) && (&commandQueue != NULL));
  if (commandQueueHandle == NULL) {
    commandQueueHandle =
        xQueueCreateStatic(COMMAND_QUEUE_LENGTH, COMMAND_QUEUE_ITEM_SIZE, commandQueueStack, &commandQueue);
  }
}

obc_error_code_t sendToCommandQueue(cmd_msg_t *cmd) {
  if (commandQueueHandle == NULL) {
    return OBC_ERR_CODE_INVALID_STATE;
  }

  if (cmd == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  if (xQueueSend(commandQueueHandle, (void *)cmd, portMAX_DELAY) == pdPASS) {
    return OBC_ERR_CODE_SUCCESS;
  }

  return OBC_ERR_CODE_QUEUE_FULL;
}

obc_error_code_t processTimeTaggedCommand(cmd_msg_t *cmd, cmd_info_t *currCmdInfo) {
  if (cmd == NULL || currCmdInfo == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  // If the timetag is in the past, throw away the command
  obc_error_code_t errCode;

  if (!cmd->isTimeTagged) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  if (cmd->timestamp < getCurrentUnixTime()) {
    return OBC_ERR_CODE_SUCCESS;
  }

  alarm_handler_event_t alarm = {.id = ALARM_HANDLER_NEW_ALARM,
                                 .alarmInfo = {
                                     .unixTime = cmd->timestamp,
                                     .callbackDef =
                                         {
                                             .cmdCallback = currCmdInfo->callback,
                                         },
                                     .type = ALARM_TYPE_TIME_TAGGED_CMD,
                                     .cmdMsg = *cmd,
                                 }};

  RETURN_IF_ERROR_CODE(sendToAlarmHandlerQueue(&alarm));
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t downlinkCmdResponse(cmd_response_t cmdResponse) {
  obc_error_code_t errCode;

  uint8_t sendBuffer[RS_DECODED_SIZE] = {0};
  packCmdResponse(&cmdResponse, sendBuffer);

  setCurrentLinkDestCallSign(CUBE_SAT_CALLSIGN, CALLSIGN_LENGTH, DEFAULT_SSID);

  // Create fec data and encode
  packed_rs_packet_t fecData = {0};
  obc_gs_error_code_t interfaceErr = rsEncode(sendBuffer, &fecData);
  if (interfaceErr != OBC_GS_ERR_CODE_SUCCESS) {
    return OBC_ERR_CODE_FEC_ENCODE_FAILURE;
  }

  // Create ax25 Frame
  unstuffed_ax25_i_frame_t unstuffedAx25Data = {0};
  interfaceErr = ax25SendIFrame((uint8_t *)&fecData.data, RS_ENCODED_SIZE, &unstuffedAx25Data);
  if (interfaceErr != OBC_GS_ERR_CODE_SUCCESS) {
    return OBC_ERR_CODE_AX25_ENCODE_FAILURE;
  }

  // Stuff the ax25 Frame
  transmit_event_t transmitEvent = {.eventID = DOWNLINK_PACKET};
  interfaceErr = ax25Stuff(unstuffedAx25Data.data, unstuffedAx25Data.length, transmitEvent.ax25Pkt.data,
                           &transmitEvent.ax25Pkt.length);

  RETURN_IF_ERROR_CODE(sendToCC1120TransmitQueue(&transmitEvent));

  return OBC_ERR_CODE_SUCCESS;
}

void obcTaskFunctionCommandMgr(void *pvParameters) {
  obc_error_code_t errCode = 0;
  uint8_t responseBuffer[MAX_RESPONSE_BUFFER_SIZE] = {0};
  cmd_response_t cmdResponse = {0};

  while (1) {
    cmd_msg_t cmd;
    if (xQueueReceive(commandQueueHandle, &cmd, portMAX_DELAY) == pdPASS) {
      cmd_info_t currCmdInfo;

      LOG_IF_ERROR_CODE(verifyCommand(&cmd, &currCmdInfo));

      if (errCode != OBC_ERR_CODE_SUCCESS) {
        continue;
      }

      if (cmd.isTimeTagged) {
        errCode = processTimeTaggedCommand(&cmd, &currCmdInfo);
      } else {
        errCode = processNonTimeTaggedCommand(&cmd, &currCmdInfo, responseBuffer);
      }

      cmdResponse.cmdId = cmd.id;
      if (errCode == OBC_ERR_CODE_SUCCESS) {
        cmdResponse.errCode = CMD_RESPONSE_SUCCESS;
      } else {
        cmdResponse.errCode = CMD_RESPONSE_ERROR;
      }

      cmdResponse.data = responseBuffer;
      cmdResponse.dataLen = MAX_RESPONSE_BUFFER_SIZE;

      LOG_IF_ERROR_CODE(downlinkCmdResponse(cmdResponse));
    }
  }
}
