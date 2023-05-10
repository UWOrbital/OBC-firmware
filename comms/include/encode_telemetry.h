#ifndef COMMS_INCLUDE_ENCODE_TELEMETRY_H_
#define COMMS_INCLUDE_ENCODE_TELEMETRY_H_

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Initializes the telemetry encoding task and queue
 * 
 */
void initTelemEncodeTask(void);

/**
 * @brief Sends a telemetry batch ID to the telemetry encoding task to begin downlinking data
 * 
 * @param telemetryBatchId - ID of the telemetry batch to send
 * @return obc_error_code_t - OBC_ERR_CODE_SUCCESS if the telemetry batch ID was successfully sent to the queue
 */
obc_error_code_t sendToTelemEncodeQueue(uint32_t telemetryBatchId);

#endif /* COMMS_INCLUDE_ENCODE_TELEMETRY_H_ */