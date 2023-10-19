#pragma once

#include "gs_errors.h"

typedef enum {
  GS_STATE_DISCONNECTED,       // GS is Idle and not doing anything
  GS_STATE_SEND_CONN,          // GS is uplinking
  GS_STATE_AWAIT_CONN_ACK,     // Switch to recv and wait for an  ACK uframe
  GS_STATE_UPLINK_COMMANDS,    // Switch to transmit to uplink all specified commands (read from cmd line for these for
                               // now)
  GS_STATE_DOWNLINK_TELEMTRY,  // once all commands are sent, switch to recv mode to recv all of our telemtry
  GS_STATE_SEND_DISC_ACK       // Trasnmit DISC Ack and then switch to disconnected.
} ground_station_state_t;

typedef enum {
  GS_EVENT_BEGIN_UPLINK,
  GS_EVENT_SEND_CONN,
  GS_EVENT_ACK_RECIEVED,
  GS_EVENT_UPLINK_FINISHED,
  GS_EVENT_DOWNLINK_FINISHED,
  GS_EVENT_DISC_SENT,
  GS_EVENT_DISC_RECEIVED,
  GS_EVENT_CONTINUE_UPLINK,
  GS_EVENT_ACK_SEND,
  GS_EVENT_CONN_RECIEVED,
  GS_EVENT_NO_ACK,
  GS_EVENT_ERROR
} ground_station_event_id_t;

gs_error_code_t updateGroundStationState(ground_station_event_t* state, ground_station_event_t event);
