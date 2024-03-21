#include "ground_state_machine.h"
#include "hackRF_utils.h"
#include "gs_errors.h"
#include "obc_gs_ax25.h"
#include <string.h>

#include "posix_freertos/include/FreeRTOS.h"
#include "FreeRTOSConfig.h"

#define MAX_AX25_BUFFER_SIZE 200

static const gs_command_t DEFAULT_COMMAND = {.command = U_FRAME_CMD_ACK, .operation = TERMINATE};
static gs_state_t groundStationState = {0};

typedef gs_error_code_t* ground_state_handler_t(void*);
static ground_state_handler_t gsCommandCallback = NULL;

static gs_error_code_t GSStateSendConnHandler();
static gs_error_code_t GSStateAwaitConnAckHandler();
static gs_error_code_t GSStateUplinkCommands();
static gs_error_code_t GSStateDowlinkTelemtry();
static gs_error_code_t GSStateSendDiscAck();

static gs_error_code_t processDownlinkedData(packed_ax25_i_frame_t data);

static ground_state_handler_t state_handlers[] = {[GS_STATE_SEND_CONN] = GSStateSendConnHandler,
                                                  [GS_STATE_AWAIT_CONN_ACK] = GSStateAwaitConnAckHandler,
                                                  [GS_STATE_UPLINK_COMMANDS] = GSStateUplinkCommands,
                                                  [GS_STATE_DOWNLINK_TELEMTRY] = GSStateDowlinkTelemtry,
                                                  [GS_STATE_SEND_DISC_ACK] = GSStateSendDiscAck};

gs_error_code_t setCommandBuffer(gs_command_t* buffer, uint8_t size) {
  memcpy(commandBuffer, buffer, size * sizeof(gs_command_t));
}

gs_error_code_t initializeGroundStation(gs_command_t* buffer) {
  if (buffer == __DARWIN_NULL) {
    return GS_ERR_CODE_INVALID_ARG;
  }
  commandBuffer = buffer;
  memcpy(buffer, &DEFAULT_COMMAND, MAX_COMMAND_BUFFER_SIZE);
}

gs_error_code_t updateGroundStationState(gs_state_t* state, gs_event_t event) {
  gs_state_t nextState = GS_STATE_DISCONNECTED;

  switch (*state) {
    case GS_STATE_DISCONNECTED:
      switch (event) {
        case GS_EVENT_UPLINK_BEGIN:
          UPDATE_STATE(nextState, GS_STATE_SEND_CONN);
        default:
          return GS_ERR_CODE_UNRECOGNIZED_TRANSITION;
      };
    case GS_STATE_SEND_CONN:
      switch (event) {
        case GS_EVENT_ERROR_RESET:
          UPDATE_STATE(nextState, GS_STATE_ERROR_CLEANUP);
        case GS_EVENT_CONTINUE:
          UPDATE_STATE(nextState, GS_STATE_AWAIT_CONN_ACK);
        default:
          return GS_ERR_CODE_UNRECOGNIZED_TRANSITION;
      };
    case GS_STATE_AWAIT_CONN_ACK:
      switch (event) {
        case GS_EVENT_NO_ACK:
          UPDATE_STATE(nextState, GS_STATE_AWAIT_CONN_ACK);
        case GS_EVENT_ERROR_RESET:
          UPDATE_STATE(nextState, GS_STATE_ERROR_CLEANUP);
        default:
          return GS_ERR_CODE_UNRECOGNIZED_TRANSITION;
      };
    case GS_STATE_UPLINK_COMMANDS:
      switch (event) {
        case GS_EVENT_UPLINK_CONTINUE:
          UPDATE_STATE(nextState, GS_STATE_UPLINK_COMMANDS);
        case GS_EVENT_UPLINK_FINISHED:
          UPDATE_STATE(nextState, GS_STATE_DOWNLINK_TELEMTRY);
        case GS_EVENT_ERROR_RESET:
          UPDATE_STATE(nextState, GS_STATE_ERROR_CLEANUP);
        default:
          return GS_ERR_CODE_UNRECOGNIZED_TRANSITION;
      };
    case GS_STATE_DOWNLINK_TELEMTRY:
      switch (event) {
        case GS_EVENT_DOWNLINK_CONTINUE:
          UPDATE_STATE(nextState, GS_STATE_DOWNLINK_TELEMTRY);
        case GS_EVENT_DOWNLINK_FINISHED:
          UPDATE_STATE(nextState, GS_STATE_SEND_DISC_ACK);
        case GS_EVENT_ERROR_RESET:
          UPDATE_STATE(nextState, GS_STATE_ERROR_CLEANUP);
        default:
          return GS_ERR_CODE_UNRECOGNIZED_TRANSITION;
      };
    case GS_STATE_SEND_DISC_ACK:
      switch (event) {
        case GS_EVENT_CONTINUE:
          UPDATE_STATE(nextState, GS_STATE_DISCONNECTED);
        case GS_EVENT_ERROR_RESET:
          UPDATE_STATE(nextState, GS_STATE_ERROR_CLEANUP);
        default:
          return GS_ERR_CODE_UNRECOGNIZED_TRANSITION;
      };
    case GS_STATE_ERROR_CLEANUP:
      UPDATE_STATE(nextState, GS_STATE_DISCONNECTED);
    default:
      return GS_ERR_CODE_INVALID_STATE;
  };

  *state = nextState;
}

static gs_error_code_t GSStateSendDiscAck() {
  packed_ax25_u_frame_t discAckPacket = {0};
  obc_gs_error_code_t errCode = ax25SendUFrame(&discAckPacket, U_FRAME_CMD_ACK, 0);
  if (errCode != OBC_GS_ERR_CODE_SUCCESS) {
    return GS_ERR_CODE_AX25_ENCODE_FAILURE;
  }
  gs_error_code_t errCode = {0};
  RETURN_IF_ERROR_CODE(transmitByteData(discAckPacket.data, discAckPacket.length));
  return GS_ERR_CODE_SUCCESS;
}

static gs_error_code_t GSStateDowlinkTelemtry() {
  packed_ax25_i_frame_t packedFrame = {0};
  gs_error_code_t errCode = {0};
  RETURN_IF_ERROR_CODE(recieveByteData(packedFrame.data, &packedFrame.length));
  RETURN_IF_ERROR_CODE(processDownlinkedData(packedFrame));
  return GS_ERR_CODE_SUCCESS;
}

static gs_error_code_t GSStateUplinkCommands() {
  if (commandBuffer == __DARWIN_NULL) return GS_ERR_CODE_INVALID_BUFFER;
  gs_command_t* bufferPtr = commandBuffer;

  for (uint32_t i = 0; i < MAX_COMMAND_BUFFER_SIZE; ++i) {
    gs_command_t buffer = bufferPtr[i];
    if (buffer.operation == TERMINATE) return;

    packed_ax25_u_frame_t frame = {0};
    obc_gs_error_code_t ax25errCode = {0};
    AX25_RETURN_IF_ERROR_CODE(ax25SendUFrame(&frame, buffer.command, 0));

    gs_error_code_t errCode = {0};
    RETURN_IF_ERROR_CODE(transmitByteData(frame.data, frame.length));
  }
  memcpy(commandBuffer, &DEFAULT_COMMAND, MAX_COMMAND_BUFFER_SIZE);
  return GS_ERR_CODE_SUCCESS;
}

static gs_error_code_t GSStateAwaitConnAckHandler() {
  for (uint8_t i = 0; i < MAX_CONNECTION_ACK_ATTEMPTS; ++i) {
    packed_ax25_i_frame_t frame = {0};
    gs_error_code_t errCode = {0};
    if (recieveByteData(frame.data, &frame.length) != GS_ERR_CODE_SUCCESS) {
      // Add delay here
      continue;
    }

    unstuffed_ax25_i_frame_t unstuffedPacket = {0};
    obc_gs_error_code_t ax25errCode = {0};
    u_frame_cmd_t cmd = {0};
    AX25_RETURN_IF_ERROR_CODE(ax25Unstuff(frame.data, frame.length, unstuffedPacket.data, &unstuffedPacket.length));
    RETURN_IF_ERROR_CODE(ax25Recv(&unstuffedPacket, &cmd));
    if (cmd == U_FRAME_CMD_ACK) {
      // Send a message along a queue or set a flag if cmd is ACK
      return;
    }
    continue;
  }
}

static gs_error_code_t GSStateSendConnHandler() {
  packed_ax25_u_frame_t frame = {0};
  obc_gs_error_code_t ax25errCode = 0;
  AX25_RETURN_IF_ERROR_CODE(ax25SendUFrame(&frame, U_FRAME_CMD_CONN, 0));

  obc_gs_error_code_t errCode = 0;
  RETURN_IF_ERROR_CODE(transmitByteData(frame.data, frame.length));
  return OBC_GS_ERR_CODE_SUCCESS;
}
