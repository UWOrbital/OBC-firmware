#include <stdio.h>
#include <stdint.h>
#include <string.h>

// Example - todo: remove
#include "command_pack.h"
#include "command_data.h"
#include "command_id.h"
#include "obc_errors.h"
#include "obc_logging.h"
#include "ax25.h"
#include "fec.h"
#include "aes128.h"
#include "aes.h"

static const uint8_t TEMP_STATIC_KEY[AES_KEY_SIZE] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                                      0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};

static struct AES_ctx ctx;
static correct_reed_solomon *rsGs = NULL;

static obc_error_code_t decodePacket(packed_ax25_i_frame_t *data, packed_rs_packet_t *rsData) {
  obc_error_code_t errCode;

  RETURN_IF_ERROR_CODE(ax25Recv(data, rsData->data, RS_ENCODED_SIZE));

  uint8_t rawAesData[RS_DECODED_SIZE] = {0};
  uint8_t decodedLength = correct_reed_solomon_decode(rsGs, rsData->data, RS_ENCODED_SIZE, rawAesData);

  if (decodedLength == -1) return OBC_ERR_CODE_CORRUPTED_MSG;

  uint8_t decryptedData[RS_DECODED_SIZE - AES_IV_SIZE] = {0};
  memcpy(aesData->iv, rawAesData, AES_IV_SIZE);
  uint8_t cipherData[AES_DECRYPTED_SIZE] = {0};
  aesData->ciphertext = cipherData;
  memcpy(aesData->ciphertext, rawAesData + AES_IV_SIZE, AES_DECRYPTED_SIZE);
  memcpy(decryptedData, aesData->ciphertext, RS_DECODED_SIZE - AES_IV_SIZE);
  AES_ctx_set_iv(&ctx, aesData->iv);
  AES_CTR_xcrypt_buffer(&ctx, decryptedData, RS_DECODED_SIZE - AES_IV_SIZE);

  for (uint8_t i = 0; i < RS_DECODED_SIZE - AES_IV_SIZE; ++i) {
    printf("%x ", decryptedData[i]);
  }
  return OBC_ERR_CODE_SUCCESS;
}

int main(void) {
  cmd_msg_t cmdMsg;
  cmdMsg.id = CMD_PING;
  cmdMsg.timestamp = 95;
  cmdMsg.isTimeTagged = true;

  uint32_t offset = 0;
  uint8_t packedCmd[MAX_CMD_MSG_SIZE] = {0};
  uint8_t numPacked = 0;
  packCmdMsg(packedCmd, &offset, &cmdMsg, &numPacked);
  printf("Packed command: ");
  for (uint8_t i = 0; i < numPacked; ++i) {
    printf("%x ", packedCmd[i]);
  }
  printf("\n");

  uint8_t iv[AES_IV_SIZE];
  memset(iv, 1, AES_IV_SIZE);

  uint8_t encryptedCmd[RS_DECODED_SIZE] = {0};

  memcpy(encryptedCmd + AES_IV_SIZE, packedCmd, numPacked);

  AES_init_ctx(&ctx, TEMP_STATIC_KEY);
  AES_ctx_set_iv(&ctx, iv);
  AES_CTR_xcrypt_buffer(&ctx, encryptedCmd + AES_IV_SIZE, AES_DECRYPTED_SIZE);

  memcpy(encryptedCmd, iv, AES_IV_SIZE);
  // memcpy(encryptedCmd + AES_IV_SIZE, packedCmd, numPacked);
  printf("Encrypted command: ");
  for (uint8_t i = 0; i < RS_DECODED_SIZE; ++i) {
    printf("%x ", encryptedCmd[i]);
  }
  printf("\n");

  packed_rs_packet_t rsPkt = {0};
  rsGs = correct_reed_solomon_create(correct_rs_primitive_polynomial_ccsds, 1, 1, 32);
  correct_reed_solomon_encode(rsGs, encryptedCmd, RS_DECODED_SIZE, rsPkt.data);
  printf("RS encoded command: ");
  for (uint8_t i = 0; i < RS_ENCODED_SIZE; ++i) {
    printf("%x ", rsPkt.data[i]);
  }
  printf("\n");

  packed_ax25_i_frame_t ax25Pkt = {0};  // Holds an AX.25 packet
  ax25SendIFrame(rsPkt.data, RS_ENCODED_SIZE, &ax25Pkt, &cubesatCallsign);

  printf("AX25 Sent Packet: ");
  for (uint16_t i = 0; i < ax25Pkt.length; ++i) {
    printf("%x ", ax25Pkt.data[i]);
  }
  printf("\n");
  packed_rs_packet_t rsData = {0};
  aes_data_t aesData = {0};
  decodePacket(&ax25Pkt, &rsData, &aesData);

  return 0;
}
