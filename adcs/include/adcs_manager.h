#ifndef ADCS_INCLUDE_ADCS_MANAGER_H_
#define ADCS_INCLUDE_ADCS_MANAGER_H_

#include "obc_errors.h"

#include <sys_common.h>



/**
 * @enum	adcs_event_id_t
 * @brief	adcs event ID enum.
 *
 * Enum containing all possible event IDs passed to the adcs event queue.
*/
typedef enum {
    ADCS_MANAGER_NULL_EVENT_ID
} adcs_event_id_t;

/**
 * @union	adcs_event_data_t
 * @brief	adcs event data union
*/
typedef union {
    int i;
    float f;
} adcs_event_data_t;

/**
 * @struct	adcs_event_t
 * @brief	adcs event struct
 *
 * Holds the message data for each event sent/received by the adcs manager queue.
*/
typedef struct {
    adcs_event_id_t eventID;
    adcs_event_data_t data;
} adcs_event_t;

/* adcs queue config */
#define ADCS_MANAGER_QUEUE_LENGTH 10U
#define ADCS_MANAGER_QUEUE_ITEM_SIZE sizeof(adcs_event_t)
#define ADCS_MANAGER_QUEUE_RX_WAIT_PERIOD pdMS_TO_TICKS(10)
#define ADCS_MANAGER_QUEUE_TX_WAIT_PERIOD pdMS_TO_TICKS(10)


/**
 * @brief	Initialize the adcs task and associated FreeRTOS constructs (queues, timers, etc.)
 */
void initADCSManager(void);

/**
 * @brief	Send an event to the ADCS queue.
 * @param	event	Event to send.
 * @return The error code
 */
obc_error_code_t sendToADCSQueue(adcs_event_t *event);

#endif /* ADCS_INCLUDE_ADCS_MANAGER_H_ */
