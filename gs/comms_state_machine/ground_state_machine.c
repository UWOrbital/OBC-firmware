#include "ground_state_machine.h"
#include "hackRF_utils.h"
#include "gs_errors.h"
#include "obc_gs_ax25.h"

#define MAX_AX25_BUFFER_SIZE 200

typedef gs_error_code_t* ground_state_handler(void);

static gs_error_code_t GSStateDisconnectedHandler();
static gs_error_code_t GSStateSendConnHandler();
static gs_error_code_t GSStateAwaitConnAckHandler();
static gs_error_code_t GSStateUplinkCommands();
static gs_error_code_t GSStateDowlinkTelemtry();
static gs_error_code_t GSStateSendDiscAck();

static ground_state_handler* state_handlers[] = {
    [GS_STATE_DISCONNECTED] = GSStateDisconnectedHandler,   [GS_STATE_SEND_CONN] = GSStateSendConnHandler,
    [GS_STATE_AWAIT_CONN_ACK] = GSStateAwaitConnAckHandler, [GS_STATE_UPLINK_COMMANDS] = GSStateUplinkCommands,
    [GS_STATE_DOWNLINK_TELEMTRY] = GSStateDowlinkTelemtry,  [GS_STATE_SEND_DISC_ACK] = GSStateSendDiscAck};

gs_error_code_t updateGroundStationState(ground_station_event_t* state, ground_station_event_t event) {
  switch (*state) {
    default:
      return GS_INVALID_STATE;
  }
}
