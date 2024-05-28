#pragma once

#include "obc_errors.h"
#include "obc_gs_ax25.h"
#include "obc_gs_fec.h"
#include "obc_gs_aes128.h"
#include "comms_manager.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  UPLINK_FLOW_DECODED_CMD = 0,
  UPLINK_FLOW_DECODED_DATA,
} uplink_flow_packet_type_t;

typedef struct {
  union {
    uint8_t data[AES_DECRYPTED_SIZE];
    comms_event_t command;
  };
  uplink_flow_packet_type_t type;
} uplink_flow_packet_t;

/**
 * @brief Fully decode a packet and store it in the command pointer
 *
 * @param ax25Data pointer to the AX.25 packet to be decoded
 * @param rsData pointer to the Reed-Solomon packet (used for data storage and contents may be overwritten)
 * @param aesData pointer to the AES packet (used for data storage and contents may be overwritten)
 * @param command pointer to the command
 */
obc_error_code_t uplinkDecodePacket(packed_ax25_i_frame_t *ax25Data, packed_rs_packet_t *rsData, aes_data_t *aesData,
                                    uplink_flow_packet_t *command);
#ifdef __cplusplus
}
#endif
