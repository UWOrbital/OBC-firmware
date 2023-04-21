#ifndef COMMS_INCLUDE_AX25_H_
#define COMMS_INCLUDE_AX25_H_

#include "obc_errors.h"
#include "fec.h"

#include <stdint.h>

#define AX25_TOTAL_FLAG_BYTES 2
#define AX25_SRC_ADDR_BYTES 7
#define AX25_DEST_ADDR_BYTES 7
#define AX25_ADDRESS_BYTES (AX25_SRC_ADDR_BYTES + AX25_DEST_ADDR_BYTES)
#define AX25_CONTROL_BYTES 2
#define AX25_PID_BYTES 1
#define AX25_FCS_BYTES 2
#define AX25_INFO_BYTES 255
#define AX25_PKT_LEN (AX25_TOTAL_FLAG_BYTES + \
                      AX25_ADDRESS_BYTES +  \
                      AX25_CONTROL_BYTES +  \
                      AX25_PID_BYTES +  \
                      AX25_FCS_BYTES +  \
                      AX25_INFO_BYTES)

typedef struct {
    uint8_t flagStart;
    uint8_t flagEnd;
    uint8_t destination[AX25_DEST_ADDR_BYTES];
    uint8_t source[AX25_SRC_ADDR_BYTES];
    uint8_t control;
    uint8_t pid;
    uint8_t data[AX25_INFO_BYTES];
    uint16_t fcs;
} ax25_packet_t;

typedef struct {
    uint8_t data[AX25_PKT_LEN];
} packed_ax25_packet_t;

/**
 * @brief adds ax.25 headers onto telemtry being downlinked
 * 
 * @param rsData reed solomon data that needs ax.25 headers added onto it
 * @param out array to store the ax.25 frame
 * 
 * @return obc_error_code_t - whether or not the ax.25 headers were successfully added
*/
obc_error_code_t ax25Send(packed_rs_packet_t *rsData, packed_ax25_packet_t *ax25Data);

/**
 * @brief strips away the ax.25 headers from a received packet
 * 
 * @param ax25Data the received ax.25 frame
 * @param rsData 255 byte array to store the reed solomon encoded data without ax.25 headers
 * 
 * @return obc_error_code_t - whether or not the ax.25 headers were successfully stripped
*/
obc_error_code_t ax25Recv(packed_ax25_packet_t *ax25Data, packed_rs_packet_t *rsData);

#endif /* COMMS_INCLUDE_AX25_H_ */
