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
#define AX25_MINIMUM_I_FRAME_LEN (AX25_TOTAL_FLAG_BYTES + \
                      AX25_ADDRESS_BYTES +  \
                      AX25_CONTROL_BYTES +  \
                      AX25_PID_BYTES +  \
                      AX25_FCS_BYTES +  \
                      AX25_INFO_BYTES)
#define AX25_MAXIMUM_PKT_LEN AX25_MINIMUM_I_FRAME_LEN*6/5
#define AX25_SUPERVISORY_FRAME_LENGTH (AX25_TOTAL_FLAG_BYTES + \
                      AX25_ADDRESS_BYTES +  \
                      AX25_CONTROL_BYTES +  \
                      AX25_PID_BYTES +  \
                      AX25_FCS_BYTES)

#define AX25_FLAG 0x7E
#define AX25_PID 0xF0U

#define AX25_S_FRAME_RR_CONTROL 0x01U
#define AX25_S_FRAME_RNR_CONTROL 0x05U
#define AX25_S_FRAME_REJ_CONTROL 0x09U
#define AX25_S_FRAME_SREJ_CONTROL 0x0DU

typedef struct {
    uint8_t data[AX25_MINIMUM_I_FRAME_LEN];
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

extern ax25_addr_t cubesatCallsign;
extern ax25_addr_t groundStationCallsign;

/**
 * @brief adds ax.25 headers onto telemetry being downlinked and stores the length of the packet in ax25Data->length
 * 
 * @param rsData data to send that needs ax.25 headers added onto it
 * @param ax25Data array to store the ax.25 frame
 * @param destAddress address of the destination for the ax25 packet
 * @param srcAddress address of the sender of the ax25 packet
 * 
 * @return obc_error_code_t - whether or not the ax.25 headers were successfully added
*/
obc_error_code_t ax25Send(uint8_t *telemData, packed_ax25_packet_t *ax25Data, ax25_addr_t *destAddress, ax25_addr_t *srcAddress);

/**
 * @brief strips away the ax.25 headers from a received packet
 * 
 * @param ax25Data the received ax.25 frame
 * @param uplinkData 255 byte array to store the received data without ax.25 headers
 * @param recvAddress address of the receiver of the ax.25 packet
 * @param uplinkDataLen length of the uplinkData array
 * 
 * @return obc_error_code_t - whether or not the ax.25 headers were successfully stripped
*/
obc_error_code_t ax25Recv(packed_ax25_packet_t *ax25Data, uint8_t *uplinkData, ax25_addr_t *recvAddress, uint8_t uplinkDataLen);

#endif /* COMMS_INCLUDE_AX25_H_ */
