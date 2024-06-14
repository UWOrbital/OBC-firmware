#include "obc_gs_command_data.h"
#include "obc_print.h"
#include "obc_scheduler_config.h"
#include "obc_gs_uplink_flow.h"
#include "obc_spi_io.h"
#include "comms_manager.h"
#include "uplink_decoder.h"
#include "obc_gs_command_pack.h"

#include <FreeRTOS.h>
#include <os_task.h>

#include <sys_common.h>
#include <sys_core.h>
#include <gio.h>
#include <sci.h>
#include <string.h>

#define STOP_ON_ERROR(text, _ret)                   \
  errCode = _ret;                                   \
  if (errCode != OBC_ERR_CODE_SUCCESS) {            \
    sciPrintf("%s returned %d\r\n", text, errCode); \
    sciPrintf("Stopping example\r\n");              \
    while (1)                                       \
      ;                                             \
  }                                                 \
  sciPrintf("%s was successful\r\n", text)

static StaticTask_t taskBuffer;
static StackType_t taskStack[1024];

static const uint8_t TEMP_STATIC_KEY[AES_KEY_SIZE] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                                      0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
static comms_state_t commsManagerState = COMMS_STATE_UPLINKING;

void vTaskFunction(void *pvParameters) {
  sciPrintf("Uplink Flow Demo\r\n");
  obc_gs_error_code_t gsErrCode;
  cmd_msg_t cmdMsg = {.id = CMD_PING, .isTimeTagged = false};
  uint32_t cmdPacketOffset = 0;

  sciPrintf("Packing Ping command\r\n");
  uint8_t packedSingleCmdSize = 0;
  uint8_t packedSingleCmd[MAX_CMD_MSG_SIZE] = {0};
  gsErrCode = packCmdMsg(packedSingleCmd, &cmdPacketOffset, &cmdMsg, &packedSingleCmdSize);
  if (gsErrCode != OBC_GS_ERR_CODE_SUCCESS) {
    sciPrintf("packCmdMsg returned %d\r\n", gsErrCode);
    sciPrintf("Stopping example\r\n");
    while (1)
      ;
  }

  uplink_flow_packet_t packet = {.data = {0}, .type = UPLINK_FLOW_DECODED_DATA};
  memcpy(packet.data, packedSingleCmd,
         packedSingleCmdSize < AES_DECRYPTED_SIZE ? packedSingleCmdSize : AES_DECRYPTED_SIZE);

  sciPrintf("Encoding packet\r\n");
  packed_ax25_i_frame_t ax25Data = {0};

  setCurrentLinkDestAddress(&groundStationCallsign);
  gsErrCode = uplinkEncodePacket(&packet, &ax25Data, TEMP_STATIC_KEY);
  if (gsErrCode != OBC_GS_ERR_CODE_SUCCESS) {
    sciPrintf("uplinkEncodePacket returned %d\r\n", gsErrCode);
    sciPrintf("Stopping example\r\n");
    while (1)
      ;
  }

  obc_error_code_t errCode;
  for (uint16_t i = 0; i < ax25Data.length; i++) {
    sciPrintf("Sending data at %d\r\n", i);
    STOP_ON_ERROR("Sending data", sendToDecodeDataQueue(&ax25Data.data[i]));
  }

  comms_event_t uplinkFinishedEvent = {.eventID = COMMS_EVENT_UPLINK_FINISHED};
  STOP_ON_ERROR("sendToCommsManagerQueue", sendToCommsManagerQueue(&uplinkFinishedEvent));
  while (1)
    ;
}

int main(void) {
  sciInit();
  spiInit();

  initSciPrint();
  initSpiMutex();

  obcSchedulerInitTask(OBC_SCHEDULER_CONFIG_ID_COMMAND_MGR);
  obcSchedulerInitTask(OBC_SCHEDULER_CONFIG_ID_COMMS_MGR);
  obcSchedulerInitTask(OBC_SCHEDULER_CONFIG_ID_COMMS_UPLINK_DECODER);
  obcSchedulerInitTask(OBC_SCHEDULER_CONFIG_ID_COMMS_DOWNLINK_ENCODER);

  obcSchedulerCreateTask(OBC_SCHEDULER_CONFIG_ID_COMMAND_MGR);
  obcSchedulerCreateTaskWithArgs(OBC_SCHEDULER_CONFIG_ID_COMMS_MGR, &commsManagerState);
  obcSchedulerCreateTask(OBC_SCHEDULER_CONFIG_ID_COMMS_UPLINK_DECODER);
  obcSchedulerCreateTask(OBC_SCHEDULER_CONFIG_ID_COMMS_DOWNLINK_ENCODER);

  xTaskCreateStatic(vTaskFunction, "Demo", 1024, NULL, 1, taskStack, &taskBuffer);

  vTaskStartScheduler();

  while (1)
    ;
}
