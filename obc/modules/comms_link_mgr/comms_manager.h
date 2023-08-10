#pragma once

#include "obc_errors.h"
#include "telemetry_manager.h"
#include "obc_gs_ax25.h"

#include <FreeRTOS.h>
#include <os_semphr.h>
#include <sys_common.h>

#define MAX_DOWNLINK_TELEM_BUFFER_SIZE 1U

#define CC1120_MUTEX_TIMEOUT pdMS_TO_TICKS(300000)

/**
 * @enum	comms_event_id_t
 * @brief	comms event ID enum.
 *
 * Enum containing all possible event IDs passed to the comms event queue.
 */
typedef enum { BEGIN_UPLINK, BEGIN_DOWNLINK } comms_event_id_t;

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

/**
 * @brief	Initialize the Comms Manager task and associated FreeRTOS constructs (queues, timers, etc.)
 */
void initCommsManager(void);

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
