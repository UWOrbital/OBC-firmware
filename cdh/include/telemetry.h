#ifndef CDH_INCLUDE_TELEMETRY_H_
#define CDH_INCLUDE_TELEMETRY_H_

#include "obc_errors.h"

#include <sys_common.h>
#include <stdio.h>

/* Telemetry task config */
#define TELEMETRY_STACK_SIZE   1024U
#define TELEMETRY_NAME         "telemetry"
#define TELEMETRY_PRIORITY     1U
#define TELEMETRY_DELAY_TICKS  pdMS_TO_TICKS(1000)

/**
 * @enum	telemetry_event_id_t
 * @brief	Telemetry event ID enum.
 *
 * Enum containing all possible event IDs passed to the telemetry event queue.
*/
typedef enum {
    TELEMETRY_NULL_EVENT_ID,
    TURN_ON_LED_EVENT_ID,
} telemetry_event_id_t;

/**
 * @union	telemetry_event_data_t
 * @brief	Telemetry event data union
*/
typedef union {
    int i;
    float f;
} telemetry_event_data_t;

/**
 * @struct	telemetry_event_t
 * @brief	Telemetry event struct
 *
 * Holds the message data for each event sent/received by the telemetry manager queue.
*/
typedef struct {
    telemetry_event_id_t eventID;
    telemetry_event_data_t data;
} telemetry_event_t;

/* Telemetry queue config */
#define TELEMETRY_QUEUE_LENGTH 10U
#define TELEMETRY_QUEUE_ITEM_SIZE sizeof(telemetry_event_t)
#define TELEMETRY_QUEUE_RX_WAIT_PERIOD pdMS_TO_TICKS(10)
#define TELEMETRY_QUEUE_TX_WAIT_PERIOD pdMS_TO_TICKS(10)

/**
 * @brief	Initialize the telemetry task and associated FreeRTOS constructs (queues, timers, etc.)
 */
void initTelemetry(void);

/**
 * @brief	Send an event to the telemetry queue.
 * @param	event	Event to send.
 * @return The error code
 */
obc_error_code_t sendToTelemetryQueue(telemetry_event_t *event);

#endif /* CDH_INCLUDE_TELEMETRY_H_ */
