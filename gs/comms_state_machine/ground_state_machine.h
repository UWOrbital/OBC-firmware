#pragma once

#include "gs_errors.h"
#include "obc_gs_ax25.h"

#define UPDATE_STATE(state, transitionState) \
  { state = transitionState; }

#define MAX_COMMAND_BUFFER_SIZE 10u
#define MAX_CONNECTION_ACK_ATTEMPTS 10u

typedef enum {
  GS_STATE_DISCONNECTED = 0x00,  // GS is Idle and not doing anything
  GS_STATE_SEND_CONN,            // GS is uplinking
  GS_STATE_AWAIT_CONN_ACK,       // Switch to recv and wait for an  ACK uframe
  GS_STATE_UPLINK_COMMANDS,    // Switch to transmit to uplink all specified commands (read from cmd line for these for
                               // now)
  GS_STATE_DOWNLINK_TELEMTRY,  // once all commands are sent, switch to recv mode to recv all of our telemtry
  GS_STATE_SEND_DISC_ACK,      // Trasnmit DISC Ack and then switch to disconnected.
  GS_STATE_ERROR_CLEANUP
} gs_state_t;

typedef enum {
  GS_EVENT_SEND_CONN = 0x00,
  GS_EVENT_ACK_RECIEVED,
  GS_EVENT_DOWNLINK_FINISHED,
  GS_EVENT_DOWNLINK_CONTINUE,
  GS_EVENT_UPLINK_BEGIN,
  GS_EVENT_UPLINK_CONTINUE,
  GS_EVENT_UPLINK_FINISHED,
  GS_EVENT_DISC_SENT,
  GS_EVENT_DISC_RECEIVED,
  GS_EVENT_ACK_SEND,
  GS_EVENT_CONN_RECIEVED,
  GS_EVENT_NO_ACK,  // Timeout?
  GS_EVENT_ERROR_RESET,
  GS_EVENT_CONTINUE  // Transition to next state if no event needed
} gs_event_t;

typedef struct {
  enum { TERMINATE, COMMAND } operation;
  u_frame_cmd_t command;
} gs_command_t;

gs_error_code_t updateGroundStationState(gs_state_t* state, gs_event_t event);
gs_error_code_t initializeGroundStation(gs_command_t* commandBuffer);
gs_error_code_t setCommandBuffer(gs_command_t* buffer, uint8_t size);
