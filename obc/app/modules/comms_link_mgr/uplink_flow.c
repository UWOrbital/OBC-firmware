#include "uplink_flow.h"
#include "obc_logging.h"

#include <string.h>

obc_error_code_t uplinkDecodePacket(packed_ax25_i_frame_t *ax25Data, packed_rs_packet_t *rsData, aes_data_t *aesData,
                                    uplink_flow_packet_t *command) {
  if (ax25Data == NULL || rsData == NULL || aesData == NULL || command == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  obc_gs_error_code_t interfaceErr;

  // perform bit unstuffing
  unstuffed_ax25_i_frame_t unstuffedPacket = {0};
  interfaceErr = ax25Unstuff(ax25Data->data, ax25Data->length, unstuffedPacket.data, &unstuffedPacket.length);
  if (interfaceErr != OBC_GS_ERR_CODE_SUCCESS) {
    return OBC_ERR_CODE_AX25_DECODE_FAILURE;
  }

  if (unstuffedPacket.data[AX25_CONTROL_BYTES_POSITION] & (0x01 << 1)) {
    // If the second least significant bit was a 1 it is a U Frame
    // copy the unstuffed data into rsData
    memcpy(rsData->data, unstuffedPacket.data + AX25_INFO_FIELD_POSITION, RS_ENCODED_SIZE);
    // clear the info field of the unstuffed packet
    memset(unstuffedPacket.data + AX25_INFO_FIELD_POSITION, 0, RS_ENCODED_SIZE);
    // decode the info field and store it in the unstuffed packet
    interfaceErr = rsDecode(rsData, unstuffedPacket.data + AX25_INFO_FIELD_POSITION, RS_DECODED_SIZE);
    if (interfaceErr != OBC_GS_ERR_CODE_SUCCESS) {
      return OBC_ERR_CODE_FEC_DECODE_FAILURE;
    }
  }

  // check for a valid ax25 frame and perform the command response if necessary
  u_frame_cmd_t recievedCmd = {0};
  interfaceErr = ax25Recv(&unstuffedPacket, &recievedCmd);
  if (interfaceErr != OBC_GS_ERR_CODE_SUCCESS) {
    return OBC_ERR_CODE_INVALID_AX25_PACKET;
  }
  if (unstuffedPacket.length != AX25_MINIMUM_I_FRAME_LEN) {
    command->type = UPLINK_FLOW_DECODED_CMD;
    if (recievedCmd == U_FRAME_CMD_CONN) {
      command->command.eventID = COMMS_EVENT_CONN_RECEIVED;
      return OBC_ERR_CODE_SUCCESS;
    } else if (recievedCmd == U_FRAME_CMD_ACK) {
      command->command.eventID = COMMS_EVENT_ACK_RECEIVED;
      return OBC_ERR_CODE_SUCCESS;
    } else if (recievedCmd == U_FRAME_CMD_DISC) {
      command->command.eventID = COMMS_EVENT_DISC_RECEIVED;
      // Why no return here in the original code?
    } else {
      return OBC_ERR_CODE_INVALID_AX25_PACKET;
    }
  }
  uint8_t ciphertext[RS_DECODED_SIZE - AES_IV_SIZE] = {0};
  aesData->ciphertext = ciphertext;

  memcpy(aesData->iv, unstuffedPacket.data + AX25_INFO_FIELD_POSITION, AES_IV_SIZE);
  memcpy(aesData->ciphertext, unstuffedPacket.data + AX25_INFO_FIELD_POSITION + AES_IV_SIZE,
         RS_DECODED_SIZE - AES_IV_SIZE);
  aesData->ciphertextLen = RS_DECODED_SIZE - AES_IV_SIZE;

  interfaceErr = aes128Decrypt(aesData, command->data, AES_DECRYPTED_SIZE);
  if (interfaceErr != OBC_GS_ERR_CODE_SUCCESS) {
    return OBC_ERR_CODE_AES_DECRYPT_FAILURE;
  }
  command->type = UPLINK_FLOW_DECODED_DATA;

  return OBC_ERR_CODE_SUCCESS;
}
