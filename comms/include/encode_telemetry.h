#ifndef COMMS_INCLUDE_ENCODE_TELEMETRY_H_
#define COMMS_INCLUDE_ENCODE_TELEMETRY_H_

#include "fec.h"
#include <stdint.h>
#include <stdbool.h>

#define PACKED_TELEM_PACKET_SIZE REED_SOLOMON_DECODED_BYTES

typedef struct {
    uint8_t data[PACKED_TELEM_PACKET_SIZE];
} packed_telem_packet_t;

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
obc_error_code_t sendToTelemEncodeQueue(uint32_t *telemetryBatchId);


#endif /* COMMS_INCLUDE_ENCODE_TELEMETRY_H_ */