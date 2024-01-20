#pragma once

#include "obc_errors.h"

#include <sys_common.h>

/**
 * @enum	pwr_event_id_t
 * @brief	pwr event ID enum.
 *
 * Enum containing all possible event IDs passed to the pwr event queue.
 */
typedef enum {
  PWR_MANAGER_OVERCURRENT_DETECTED,
} pwr_event_id_t;

/**
 * @struct	pwr_event_t
 * @brief	pwr event struct
 *
 * Holds the message data for each event sent/received by the pwr manager queue.
 */
typedef struct {
  pwr_event_id_t eventID;
} pwr_event_t;

/**
 * @brief	Send an event to the PWR Manager queue.
 * @param	event	Event to send.
 * @return The error code
 */
obc_error_code_t sendToPWRQueue(pwr_event_t *event);

/**
 * @brief Handle the overcurrent interrupt.
 *
 */
void overcurrentInterruptCallback(void);
