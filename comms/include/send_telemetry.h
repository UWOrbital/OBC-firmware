#ifndef COMMS_INCLUDE_COMMS_TX_H_
#define COMMS_INCLUDE_COMMS_TX_H_

#include "fec.h"

#define PACKED_TELEM_PACKET_SIZE REED_SOLOMON_DECODED_BYTES

typedef struct {
    uint8_t data[PACKED_TELEM_PACKET_SIZE];
} packed_telem_t;

/**
 * @brief Queues AX.25 packets into the CC1120 transmit queue
 * 
 * @param telemFileId - ID of the telemetry file to send
 */
void sendTelemetry(uint32_t telemFileId);

/**
 * @brief Initialize the CC1120 transmit task and queue
 * 
 */
void initCC1120TransmitTask(void);

#endif /* COMMS_INCLUDE_COMMS_TX_H_ */
