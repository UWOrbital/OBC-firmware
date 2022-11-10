#ifndef EPS_INCLUDE_EPS_MANAGER_H_
#define EPS_INCLUDE_EPS_MANAGER_H_

#include <sys_common.h>

/* EPS Manager task config */
#define EPS_MANAGER_STACK_SIZE   1024
#define EPS_MANAGER_NAME         "eps_manager"
#define EPS_MANAGER_PRIORITY     1

/**
 * @enum	eps_event_id_t
 * @brief	eps event ID enum.
 *
 * Enum containing all possible event IDs passed to the eps event queue.
*/
typedef enum {
    EPS_MANAGER_NULL_EVENT_ID
} eps_event_id_t;

/**
 * @union	eps_event_data_t
 * @brief	eps event data union
*/
typedef union {
    int i;
    float f;
} eps_event_data_t;

/**
 * @struct	eps_event_t
 * @brief	eps event struct
 *
 * Holds the message data for each event sent/received by the eps manager queue.
*/
typedef struct {
    eps_event_id_t eventID;
    eps_event_data_t data;
} eps_event_t;

/* EPS queue config */
#define EPS_MANAGER_QUEUE_LENGTH 10
#define EPS_MANAGER_QUEUE_ITEM_SIZE sizeof(eps_event_t)
#define EPS_MANAGER_QUEUE_WAIT_PERIOD pdMS_TO_TICKS(10)

/**
 * @brief	Initialize the EPS Manager task and associated FreeRTOS constructs (queues, timers, etc.)
 */
void initEPS(void);

/**
 * @brief	Send an event to the EPS Manager queue.
 * @param	event	Event to send.
 * @return	1 if successful, 0 otherwise.
 */
uint8_t sendToEPSQueue(eps_event_t *event);

#endif /* EPS_INCLUDE_EPS_MANAGER_H_ */
