#ifndef COMMS_INCLUDE_AX25_H_
#define COMMS_INCLUDE_AX25_H_

#include <stdint.h>
#include "obc_errors.h"
#include "ax25.h"

#define AX25_TOTAL_FLAG_BYTES 2
#define AX25_ADDRESS_BYTES 16
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
    uint8_t flag;
    uint8_t destination[AX25_ADDRESS_BYTES];
    uint8_t source[AX25_ADDRESS_BYTES];
    uint8_t control;
    uint8_t pid;
    uint8_t data[AX25_INFO_BYTES];
    uint16_t fcs;
} ax25_packet_t;

typedef union {
    uint8_t data[AX25_PKT_LEN];
} packed_ax25_packet_t;

obc_error_code_t ax25Frame(packed_rs_packet_t *rsData, packed_ax25_packet_t *ax25Data);

#endif /* COMMS_INCLUDE_AX25_H_ */
