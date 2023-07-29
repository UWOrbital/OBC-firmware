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

/**
 * @brief	Initialize the Comms Manager task and associated FreeRTOS constructs (queues, timers, etc.)
 */
void initCommsManager(void);

/**
 * @brief sets the encodeFlag to the specified value
 *
 * @param val whether to set the flag to true or false
 */
void setEncodeFlag(bool val);

/**
 * @brief	Send an event to the Comms Manager queue.
 * @param	event	Event to send.
 * @return The error code
 */
obc_error_code_t sendToCommsQueue(comms_event_t *event);

/**
 * @brief Sends an AX.25 packet to the CC1120 transmit queue
 *
 * @param ax25Pkt - Pointer to the AX.25 packet to send
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if the packet was sent to the queue
 */
obc_error_code_t sendToCC1120TransmitQueue(packed_ax25_i_frame_t *ax25Pkt);
