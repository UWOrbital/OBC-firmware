#ifndef CDH_INCLUDE_SUPERVISOR_H_
#define CDH_INCLUDE_SUPERVISOR_H_

#include "obc_errors.h"

#include <sys_common.h>

/**
 * @enum	supervisor_event_id_t
 * @brief	Supervisor event ID enum.
 *
 * Enum containing all possible event IDs passed to the supervisor event queue.
*/
typedef enum {
    SUPERVISOR_NULL_EVENT_ID,
} supervisor_event_id_t;

/**
 * @union	supervisor_event_data_t
 * @brief	Supervisor event data union
*/
typedef union {
    int i;
    float f;
} supervisor_event_data_t;

/**
 * @struct	supervisor_event_t
 * @brief	Supervisor event struct
 *
 * Holds the message data for each event sent/received by the supervisor manager queue.
*/
typedef struct {
    supervisor_event_id_t eventID;
    supervisor_event_data_t data;
} supervisor_event_t;

/**
 * @brief	Initialize the supervisor task and associated FreeRTOS constructs (queues, timers, etc.)
 */
void initSupervisor(void);

/**
 * @brief	Send an event to the supervisor queue.
 * @param	event	Event to send.
 * @return The error code
 */
obc_error_code_t sendToSupervisorQueue(supervisor_event_t *event);

#endif /* CDH_INCLUDE_SUPERVISOR_H_ */
