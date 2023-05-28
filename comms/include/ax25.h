#ifndef COMMS_INCLUDE_AX25_H_
#define COMMS_INCLUDE_AX25_H_

#include "obc_errors.h"
#include "fec.h"

#include <stdint.h>

#define AX25_START_FLAG_BYTES 1
#define AX25_END_FLAG_BYTES 1
#define AX25_TOTAL_FLAG_BYTES 2
#define AX25_SRC_ADDR_BYTES 7
#define AX25_DEST_ADDR_BYTES 7
#define AX25_ADDRESS_BYTES (AX25_SRC_ADDR_BYTES + AX25_DEST_ADDR_BYTES)
#define AX25_CONTROL_BYTES 2
#define AX25_PID_BYTES 1
#define AX25_FCS_BYTES 2
#define AX25_INFO_BYTES 255
#define AX25_MINIMUM_PKT_LEN (AX25_TOTAL_FLAG_BYTES + \
                      AX25_ADDRESS_BYTES +  \
                      AX25_CONTROL_BYTES +  \
                      AX25_PID_BYTES +  \
                      AX25_FCS_BYTES +  \
                      AX25_INFO_BYTES)
#define AX25_MAXIMUM_PKT_LEN AX25_MINIMUM_PKT_LEN*6/5
#define AX25_SUPERVISORY_FRAME_LENGTH (AX25_TOTAL_FLAG_BYTES + \
                      AX25_ADDRESS_BYTES +  \
                      AX25_CONTROL_BYTES +  \
                      AX25_PID_BYTES +  \
                      AX25_FCS_BYTES)

#define AX25_FLAG 0x7E
#define AX25_PID 0xF0U

/* Mock CubeSat callsign */
#define AX25_CUBESAT_CALLSIGN_BYTE_1 0xFFU
#define AX25_CUBESAT_CALLSIGN_BYTE_2 0xFFU
#define AX25_CUBESAT_CALLSIGN_BYTE_3 0xFFU
#define AX25_CUBESAT_CALLSIGN_BYTE_4 0xFFU
#define AX25_CUBESAT_CALLSIGN_BYTE_5 0xFFU
#define AX25_CUBESAT_CALLSIGN_BYTE_6 0xFFU
#define AX25_CUBESAT_CALLSIGN_BYTE_7 0xFFU

#define AX25_S_FRAME_RR_CONTROL 0x01U
#define AX25_S_FRAME_RNR_CONTROL 0x05U
#define AX25_S_FRAME_REJ_CONTROL 0x09U
#define AX25_S_FRAME_SREJ_CONTROL 0x0DU

typedef struct {
    uint8_t data[AX25_MINIMUM_PKT_LEN];
    uint16_t length;
} unstuffed_ax25_packet_t;

typedef struct {
    uint8_t data[AX25_MAXIMUM_PKT_LEN];
    uint16_t length;
} packed_ax25_packet_t;

typedef struct {
    uint8_t data[AX25_DEST_ADDR_BYTES];
    uint8_t length;
} ax25_addr_t;

/**
 * @brief adds ax.25 headers onto telemetry being downlinked and stores the length of the packet in az25Data->length
 * 
 * @param rsData reed solomon data that needs ax.25 headers added onto it
 * @param ax25Data array to store the ax.25 frame
 * @param destAddress address of the destination for the ax25 packet
 * 
 * @return obc_error_code_t - whether or not the ax.25 headers were successfully added
*/
obc_error_code_t ax25Send(packed_rs_packet_t *rsData, packed_ax25_packet_t *ax25Data, ax25_addr_t* destAddress);

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
