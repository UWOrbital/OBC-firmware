#ifndef COMMS_INCLUDE_COMMS_MANAGER_H_
#define COMMS_INCLUDE_COMMS_MANAGER_H_

#include "obc_errors.h"
#include "telemetry_manager.h"

#include <sys_common.h>

#define MAX_DOWNLINK_TELEM_BUFFER_SIZE 1U

/**
 * @enum	comms_event_id_t
 * @brief	comms event ID enum.
 *
 * Enum containing all possible event IDs passed to the comms event queue.
*/
typedef enum {
    BEGIN_UPLINK,
    DOWNLINK_TELEMETRY_FILE,
    DOWNLINK_DATA_BUFFER
} comms_event_id_t;

typedef struct {
    telemetry_data_t telemData[MAX_DOWNLINK_TELEM_BUFFER_SIZE];
    uint8_t bufferSize;
} telemetry_data_buffer_t;

/**
 * @struct	comms_event_t
 * @brief	comms event struct
 *
 * Holds the message data for each event sent/received by the comms manager queue.
*/
typedef struct {
    comms_event_id_t eventID;
    union {
        uint32_t telemetryBatchId;
        telemetry_data_buffer_t telemetryDataBuffer;
    };
} comms_event_t;

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
