#ifndef COMMS_INCLUDE_SEND_TELEMETRY_H_
#define COMMS_INCLUDE_SEND_TELEMETRY_H_

#include "ax25.h"

/**
 * @brief Initialize the CC1120 transmit task and queue
 * 
 */
void initCC1120TransmitTask(void);

/**
 * @brief Sends an AX.25 packet to the CC1120 transmit queue
 * 
 * @param ax25Pkt - Pointer to the AX.25 packet to send
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if the packet was sent to the queue
 */
obc_error_code_t sendToCC1120TransmitQueue(packed_ax25_packet_t *ax25Pkt);

#endif /* COMMS_INCLUDE_COMMS_TX_H_ */
