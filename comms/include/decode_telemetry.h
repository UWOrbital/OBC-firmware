#ifndef COMMS_INCLUDE_DECODE_TELEMETRY_H
#define COMMS_INCLUDE_DECODE_TELEMETRY_H

#include "obc_errors.h"
#include "ax25.h"

#include <stdbool.h>

/**
 * @brief parses the completely decoded data and sends it to the command manager and detects end of transmission
 * 
 * @param cmdBytes 256 byte array storing the completely decoded data
 * 
 * @return obc_error_code_t - whether or not the data was successfullysent to the command manager
*/
obc_error_code_t handleCommands(uint8_t *cmdBytes);

/**
 * @brief initializes the decode data pipeline task
 * 
 * @return void
*/
void initDecodeTask(void);

/**
 * @brief send a received packet to the decode data pipeline to be sent to command manager
 * 
 * @param data array storing the packet
 * 
 * @return obc_error_code_t - whether or not the packet was successfully sent to the queue
*/
obc_error_code_t sendToDecodeDataQueue(packed_ax25_packet_t *data);

#endif /* COMMS_INCLUDE_DECODE_TELEMETRY_H */
