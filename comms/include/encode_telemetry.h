#ifndef COMMS_INCLUDE_ENCODE_TELEMETRY_H_
#define COMMS_INCLUDE_ENCODE_TELEMETRY_H_

#include <stdint.h>
#include <stdbool.h>

#include "obc_errors.h"
#include "comms_manager.h"

/**
 * @brief Initializes the telemetry encoding task and queue
 * 
 */
void initTelemEncodeTask(void);

/**
 * @brief Sends downlink data to encoding task queue
 * 
 * @param queueMsg - Includes command ID, and either a telemetry batch ID or a telemetry_data_t array
 * @return obc_error_code_t - OBC_ERR_CODE_SUCCESS if the telemetry batch ID was successfully sent to the queue
 */
obc_error_code_t sendToDownlinkQueue(comms_event_t *queueMsg);

#endif /* COMMS_INCLUDE_ENCODE_TELEMETRY_H_ */