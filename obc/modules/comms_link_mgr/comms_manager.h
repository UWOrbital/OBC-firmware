#pragma once

#include "obc_errors.h"
#include "telemetry_manager.h"
#include "obc_gs_ax25.h"

#include <FreeRTOS.h>
#include <os_semphr.h>
#include <sys_common.h>

#define MAX_DOWNLINK_TELEM_BUFFER_SIZE 1U

/**
 * @enum	comms_event_id_t
 * @brief	comms event ID enum.
 *
 * Enum containing all possible event IDs passed to the comms event queue.
 */
typedef enum {
  COMMS_EVENT_BEGIN_UPLINK,
  COMMS_EVENT_CONN_RECEIVED,
  COMMS_EVENT_ACK_SENT,
  COMMS_EVENT_UPLINK_FINISHED,
  COMMS_EVENT_DOWNLINK_FINISHED,
  COMMS_EVENT_DISC_SENT,
  COMMS_EVENT_DISC_RECEIVED,
  COMMS_EVENT_CONTINUE_UPLINK,
  COMMS_EVENT_ACK_RECEIVED,
  COMMS_EVENT_ENTER_EMERG,
  COMMS_EVENT_EMERG_INITIALIZED,
  COMMS_EVENT_CONN_SENT,
  COMMS_EVENT_NO_ACK,
  COMMS_EVENT_ERROR
} comms_event_id_t;

/**
 * @struct	comms_event_t
 * @brief	comms event struct
 *
 * Holds the message data for each event sent/received by the comms manager queue.
 */
typedef struct {
  comms_event_id_t eventID;
} comms_event_t;

typedef enum { DOWNLINK_PACKET, END_DOWNLINK } transmit_event_id_t;

typedef struct {
  transmit_event_id_t eventID;
  packed_ax25_i_frame_t ax25Pkt;
} transmit_event_t;

typedef enum {
  COMMS_STATE_DISCONNECTED = 0,
  COMMS_STATE_AWAITING_CONN,
  COMMS_STATE_SENDING_CONN,
  COMMS_STATE_SENDING_DISC,
  COMMS_STATE_SENDING_ACK,
  COMMS_STATE_AWAITING_ACK_DISC,
  COMMS_STATE_AWAITING_ACK_CONN,
  COMMS_STATE_UPLINKING,
  COMMS_STATE_DOWNLINKING,
  COMMS_STATE_ENTERING_EMERGENCY,
  COMMS_STATE_EMERGENCY_UPLINK,
} comms_state_t;

/**
 * @brief	Initialize the Comms Manager task and associated FreeRTOS constructs (queues, timers, etc.)
 */
void initCommsManager(comms_state_t *commsState);

/**
 * @brief	Send an event to the Comms Manager queue.
 * @param	event	Event to send.
 * @return The error code
 */
obc_error_code_t sendToCommsManagerQueue(comms_event_t *event);

/**
 * @brief Send an event to the front of the Comms Manager queue
 *
 * @param event Event to send
 *
 * @return obc_error_code_t whether or not the event was successfully sent to queue
 */
obc_error_code_t sendToFrontCommsManagerQueue(comms_event_t *event);

/**
 * @brief Sends an AX.25 packet to the CC1120 transmit queue
 *
 * @param ax25Pkt - Pointer to the AX.25 packet to send
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if the packet was sent to the queue
 */
obc_error_code_t sendToCC1120TransmitQueue(transmit_event_t *event);
