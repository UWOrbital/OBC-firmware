#include "obc_gs_uplink_flow.h"
#include "obc_gs_ax25.h"
#include "obc_gs_errors.h"
#include "obc_gs_fec.h"
#include "obc_gs_aes128.h"

#include <string.h>

obc_gs_error_code_t uplinkDecodePacket(packed_ax25_i_frame_t *ax25Data, uplink_flow_packet_t *command) {
  if (ax25Data == NULL || command == NULL) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }

  packed_rs_packet_t rsData = {0};
  obc_gs_error_code_t interfaceErr;

  // perform bit unstuffing
  unstuffed_ax25_i_frame_t unstuffedPacket = {0};
  interfaceErr = ax25Unstuff(ax25Data->data, ax25Data->length, unstuffedPacket.data, &unstuffedPacket.length);
  if (interfaceErr != OBC_GS_ERR_CODE_SUCCESS) {
    return interfaceErr;
  }

  if (unstuffedPacket.data[AX25_CONTROL_BYTES_POSITION] & (0x01 << 1)) {
    // If the second least significant bit was a 1 it is a U Frame
    // copy the unstuffed data into rsData
    memcpy(rsData.data, unstuffedPacket.data + AX25_INFO_FIELD_POSITION, RS_ENCODED_SIZE);
    // clear the info field of the unstuffed packet
    memset(unstuffedPacket.data + AX25_INFO_FIELD_POSITION, 0, RS_ENCODED_SIZE);
    // decode the info field and store it in the unstuffed packet
    interfaceErr = rsDecode(&rsData, unstuffedPacket.data + AX25_INFO_FIELD_POSITION, RS_DECODED_SIZE);
    if (interfaceErr != OBC_GS_ERR_CODE_SUCCESS) {
      return interfaceErr;
    }
  }

  // check for a valid ax25 frame and return the command response if necessary
  u_frame_cmd_t recievedCmd = {0};
  interfaceErr = ax25Recv(&unstuffedPacket, &recievedCmd);
  if (interfaceErr != OBC_GS_ERR_CODE_SUCCESS) {
    return interfaceErr;
  }
  if (unstuffedPacket.length != AX25_MINIMUM_I_FRAME_LEN) {
    if (recievedCmd != U_FRAME_CMD_CONN && recievedCmd != U_FRAME_CMD_ACK && recievedCmd != U_FRAME_CMD_DISC) {
      return OBC_GS_ERR_CODE_INVALID_AX25_PACKET;
    }
    command->type = UPLINK_FLOW_DECODED_UFRAME_CMD;
    command->command = recievedCmd;
    return OBC_GS_ERR_CODE_SUCCESS;
  }
  // Decrypt packet
  uint8_t ciphertext[AES_DECRYPTED_SIZE] = {0};
  aes_data_t aesData = {0};
  aesData.ciphertext = ciphertext;

  memcpy(aesData.iv, unstuffedPacket.data + AX25_INFO_FIELD_POSITION, AES_IV_SIZE);
  memcpy(aesData.ciphertext, unstuffedPacket.data + AX25_INFO_FIELD_POSITION + AES_IV_SIZE,
         RS_DECODED_SIZE - AES_IV_SIZE);
  aesData.ciphertextLen = AES_DECRYPTED_SIZE;

  interfaceErr = aes128Decrypt(&aesData, command->data, AES_DECRYPTED_SIZE);
  if (interfaceErr != OBC_GS_ERR_CODE_SUCCESS) {
    return interfaceErr;
  }
  command->type = UPLINK_FLOW_DECODED_DATA;

  return OBC_GS_ERR_CODE_SUCCESS;
}

obc_gs_error_code_t uplinkEncodePacket(uplink_flow_packet_t *command, packed_ax25_i_frame_t *ax25Data,
                                       const uint8_t *aesKey) {
  if (command == NULL || ax25Data == NULL || aesKey == NULL) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }
  // TODO: Handle the case where the command is a U frame, then remove this guard
  if (command->type != UPLINK_FLOW_DECODED_DATA) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }
  obc_gs_error_code_t interfaceErr;

  // Encrypt the data
  uint8_t ciphertext[AES_DECRYPTED_SIZE] = {0};
  aes_data_t aesData = {0};
  aesData.ciphertext = ciphertext;
  aesData.ciphertextLen = AES_DECRYPTED_SIZE;
  memcpy(aesData.iv, aesKey, AES_IV_SIZE);

  interfaceErr = aes128Encrypt(command->data, AES_DECRYPTED_SIZE, &aesData);
  if (interfaceErr != OBC_GS_ERR_CODE_SUCCESS) {
    return interfaceErr;
  }

  // Format data for AX.25
  uint8_t ax25InfoField[RS_ENCODED_SIZE] = {0};
  memcpy(ax25InfoField, aesData.iv, AES_IV_SIZE);
  memcpy(ax25InfoField + AES_IV_SIZE, aesData.ciphertext,
         AES_DECRYPTED_SIZE < RS_ENCODED_SIZE - AES_IV_SIZE ? AES_DECRYPTED_SIZE : RS_ENCODED_SIZE - AES_IV_SIZE);

  // Send I Frame
  unstuffed_ax25_i_frame_t unstuffedAx25Pkt = {0};
  interfaceErr = ax25SendIFrame(ax25InfoField, RS_ENCODED_SIZE, &unstuffedAx25Pkt);
  if (interfaceErr != OBC_GS_ERR_CODE_SUCCESS) {
    return interfaceErr;
  }

  packed_rs_packet_t rsData = {0};
  interfaceErr = rsEncode(unstuffedAx25Pkt.data + AX25_INFO_FIELD_POSITION, &rsData);
  if (interfaceErr != OBC_GS_ERR_CODE_SUCCESS) {
    return interfaceErr;
  }

  // Stuff the data, this should be the last step
  interfaceErr = ax25Stuff(unstuffedAx25Pkt.data, unstuffedAx25Pkt.length, ax25Data->data, &ax25Data->length);
  if (interfaceErr != OBC_GS_ERR_CODE_SUCCESS) {
    return interfaceErr;
  }

  ax25Data->data[0] = AX25_FLAG;
  ax25Data->data[ax25Data->length - 1] = AX25_FLAG;

  return OBC_GS_ERR_CODE_SUCCESS;
}
