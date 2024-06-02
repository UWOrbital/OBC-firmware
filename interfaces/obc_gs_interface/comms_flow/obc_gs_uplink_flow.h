#pragma once

#include "obc_gs_aes128.h"
#include "obc_gs_errors.h"
#include "obc_gs_ax25.h"
#include "obc_gs_fec.h"  // Needed for AES_DECRYPTED_SIZE macro

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  UPLINK_FLOW_DECODED_UFRAME_CMD = 0,
  UPLINK_FLOW_DECODED_DATA,
} uplink_flow_packet_type_t;

typedef struct {
  union {
    u_frame_cmd_t command;
    uint8_t data[AES_DECRYPTED_SIZE];
  };
  uplink_flow_packet_type_t type;
} uplink_flow_packet_t;

/**
 * @brief Fully decode a packet and store it in the command pointer
 *
 * @param ax25Data pointer to the AX.25 packet to be decoded
 * @param rsData pointer to the Reed-Solomon packet (used for data storage and contents may be overwritten)
 * @param aesData pointer to the AES packet (used for data storage and contents may be overwritten)
 * @param command pointer to the command (result)
 *
 * @return OBC_GS_ERR_CODE_SUCCESS if the packet was successfully decoded, otherwise an error code
 */
obc_gs_error_code_t uplinkDecodePacket(packed_ax25_i_frame_t *ax25Data, uplink_flow_packet_t *command);

/**
 * @brief Fully encode a packet and store it in the ax25Data pointer
 *
 * @param command pointer to the command to be encoded
 * @param ax25Data pointer to AX.25 packet (result)
 * @param aesKey pointer to the AES key used for encryption
 *
 * @note The AES key must be 16 bytes long
 * @note Callsign must be set before calling this function
 *
 * @return OBC_GS_ERR_CODE_SUCCESS if the packet was successfully encoded, otherwise an error code
 */
obc_gs_error_code_t uplinkEncodePacket(uplink_flow_packet_t *command, packed_ax25_i_frame_t *ax25Data,
                                       const uint8_t *aesKey);

#ifdef __cplusplus
}
#endif
