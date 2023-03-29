#ifndef COMMS_INCLUDE_REED_SOLOMON_H_
#define COMMS_INCLUDE_REED_SOLOMON_H_

#include <stdint.h>
#include "send_telemetry.h"
#include "obc_errors.h"

#define REED_SOLOMON_DECODED_BYTES 223U
#define REED_SOLOMON_ENCODED_BYTES 255U

typedef struct {
    uint8_t data[REED_SOLOMON_ENCODED_BYTES];
} packed_rs_packet_t;

obc_error_code_t rsEncode(packed_telem_t *telemData, packed_rs_packet_t *rsData);

#endif /* COMMS_INCLUDE_REED_SOLOMON_H_ */
