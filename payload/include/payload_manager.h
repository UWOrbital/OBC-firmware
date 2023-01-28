#ifndef PAYLOAD_INCLUDE_PAYLOAD_MANAGER_H_
#define PAYLOAD_INCLUDE_PAYLOAD_MANAGER_H_

#include "obc_errors.h"

#include <sys_common.h>

/* Payload Manager task config */
#define PAYLOAD_MANAGER_STACK_SIZE   1024U
#define PAYLOAD_MANAGER_NAME         "payload_manager"
#define PAYLOAD_MANAGER_PRIORITY     1U

/**
 * @enum	payload_event_id_t
 * @brief	payload event ID enum.
 *
 * Enum containing all possible event IDs passed to the payload event queue.
*/
typedef enum {
    PAYLOAD_MANAGER_NULL_EVENT_ID
} payload_event_id_t;

/**
 * @union	payload_event_data_t
 * @brief	payload event data union
*/
typedef union {
    int i;
    float f;
} payload_event_data_t;

/**
 * @struct	payload_event_t
 * @brief	payload event struct
 *
 * Holds the message data for each event sent/received by the payload manager queue.
*/
typedef struct {
    payload_event_id_t eventID;
    payload_event_data_t data;
} payload_event_t;

/* payload queue config */
#define PAYLOAD_MANAGER_QUEUE_LENGTH 10U
#define PAYLOAD_MANAGER_QUEUE_ITEM_SIZE sizeof(payload_event_t)
#define PAYLOAD_MANAGER_QUEUE_RX_WAIT_PERIOD pdMS_TO_TICKS(10)
#define PAYLOAD_MANAGER_QUEUE_TX_WAIT_PERIOD pdMS_TO_TICKS(10)

/**
 * @brief	Initialize the Payload Manager task and associated FreeRTOS constructs (queues, timers, etc.)
 */
void initPayloadManager(void);

/**
 * @brief	Send an event to the Payload Manager queue.
 * @param	event	Event to send.
 * @return The error code
 */
obc_error_code_t sendToPayloadQueue(payload_event_t *event);

#endif /* PAYLOAD_INCLUDE_PAYLOAD_MANAGER_H_ */
