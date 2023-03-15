#ifndef COMMS_INCLUDE_COMMS_MANAGER_H_
#define COMMS_INCLUDE_COMMS_MANAGER_H_

#include "obc_errors.h"

#include <sys_common.h>


/**
 * @enum	comms_event_id_t
 * @brief	comms event ID enum.
 *
 * Enum containing all possible event IDs passed to the comms event queue.
*/
typedef enum {
    COMMS_MANAGER_NULL_EVENT_ID
} comms_event_id_t;

/**
 * @union	comms_event_data_t
 * @brief	comms event data union
*/
typedef union {
    int i;
    float f;
} comms_event_data_t;

/**
 * @struct	comms_event_t
 * @brief	comms event struct
 *
 * Holds the message data for each event sent/received by the comms manager queue.
*/
typedef struct {
    comms_event_id_t eventID;
    comms_event_data_t data;
} comms_event_t;

// /* Comms Manager event queue config */
#define COMMS_MANAGER_QUEUE_LENGTH 10U
#define COMMS_MANAGER_QUEUE_ITEM_SIZE sizeof(comms_event_t)
#define COMMS_MANAGER_QUEUE_RX_WAIT_PERIOD pdMS_TO_TICKS(10)
#define COMMS_MANAGER_QUEUE_TX_WAIT_PERIOD pdMS_TO_TICKS(10)

/**
 * @brief	Initialize the Comms Manager task and associated FreeRTOS constructs (queues, timers, etc.)
 */
void initCommsManager(void);

/**
 * @brief	Send an event to the Comms Manager queue.
 * @param	event	Event to send.
 * @return The error code
 */
obc_error_code_t sendToCommsQueue(comms_event_t *event);

#endif /* COMMS_INCLUDE_COMMS_MANAGER_H_ */
