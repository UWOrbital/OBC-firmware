#include "obc_gs_aes128.h"
#include "obc_gs_errors.h"

#include <aes.h>

#include <stdint.h>
#include <string.h>

// Store the AES context
static struct AES_ctx ctx;

obc_gs_error_code_t aes128Decrypt(aes_data_t *aesData, uint8_t *output, uint8_t outputBufferLen) {
  if (aesData == NULL) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }

  if (output == NULL) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }

  if (outputBufferLen != aesData->ciphertextLen) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }

  memcpy(output, aesData->ciphertext, aesData->ciphertextLen);
  AES_ctx_set_iv(&ctx, aesData->iv);
  AES_CTR_xcrypt_buffer(&ctx, output, aesData->ciphertextLen);

  return OBC_GS_ERR_CODE_SUCCESS;
}

obc_gs_error_code_t aes128Encrypt(const uint8_t *data, uint8_t dataBufferLen, aes_data_t *output) {
  if (data == NULL) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }

  if (output == NULL) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }

  if (dataBufferLen != output->ciphertextLen) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }

  memcpy(output->ciphertext, data, dataBufferLen);
  AES_ctx_set_iv(&ctx, output->iv);
  AES_CTR_xcrypt_buffer(&ctx, output->ciphertext, output->ciphertextLen);
  return OBC_GS_ERR_CODE_SUCCESS;
}
/**
 * @brief Initializes the AES context
 *
 * @param key - The key to decrypt the AES blocks with
 * @return obc_gs_error_code_t - whether or not the context was successfully initialized
 */
obc_gs_error_code_t initializeAesCtx(const uint8_t *key) {
  if (key == NULL) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }

  AES_init_ctx(&ctx, key);

  return OBC_GS_ERR_CODE_SUCCESS;
}
