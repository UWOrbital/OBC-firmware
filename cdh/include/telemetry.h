#ifndef CDH_INCLUDE_TELEMETRY_H_
#define CDH_INCLUDE_TELEMETRY_H_

#include <sys_common.h>
#include <stdio.h>

/* Telemetry task config */
#define TELEMETRY_STACK_SIZE   1024
#define TELEMETRY_NAME         "telemetry"
#define TELEMETRY_PRIORITY     1
#define TELEMETRY_DELAY_TICKS  1000/portTICK_PERIOD_MS

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
#define TELEMETRY_QUEUE_LENGTH 10
#define TELEMETRY_QUEUE_ITEM_SIZE sizeof(telemetry_event_t)
#define TELEMETRY_QUEUE_WAIT_PERIOD 10/portTICK_PERIOD_MS

/**
 * @brief	Initialize the telemetry task and associated FreeRTOS constructs (queues, timers, etc.)
 */
void initTelemetry(void);

/**
 * @brief	Send an event to the telemetry queue.
 * @param	event	Event to send.
 * @return	1 if successful, 0 otherwise.
 */
uint8_t sendToTelemetryQueue(telemetry_event_t *event);

/**
 * @brief	Send contents from queue as raw bytes into file
 * 
 * @return	1 if successful, 0 otherwise.
 */
uint8_t sendTelemetryToFile(FILE *telFile, telemetry_event_t queueMsg);

#endif /* CDH_INCLUDE_TELEMETRY_H_ */
