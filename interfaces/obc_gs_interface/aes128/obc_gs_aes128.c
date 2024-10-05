#include "obc_gs_aes128.h"
#include "obc_gs_errors.h"

#include <gcm.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

static mbedtls_gcm_context gcm_ctx;

obc_gs_error_code_t initializeAesCtx(const uint8_t *key) {
  if (key == NULL) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }

  mbedtls_gcm_init(&gcm_ctx);

  int ret = mbedtls_gcm_setkey(&gcm_ctx, MBEDTLS_CIPHER_ID_AES, key, AES_KEY_BITS);
  if (ret == 0) {
    return OBC_GS_ERR_CODE_SUCCESS;
  } else if (ret == MBEDTLS_ERR_GCM_BAD_INPUT) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  } else {
    return OBC_GS_ERR_CODE_INIT_FAIL;
  }
}
// void initializeAesCtx(const uint8_t *key) {
//   mbedtls_gcm_init(&gcm_ctx);
//   mbedtls_gcm_setkey(&gcm_ctx, MBEDTLS_CIPHER_ID_AES, key, AES_KEY_BITS);
// }

obc_gs_error_code_t gcmEncrypt(aes_data_t *aesData, const uint8_t *plaintext, size_t plaintextLen,
                               const uint8_t *additionalData, size_t additionalDataLen, uint8_t *output) {
  if (aesData == NULL || plaintext == NULL || output == NULL) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }

  int result =
      mbedtls_gcm_crypt_and_tag(&gcm_ctx, MBEDTLS_GCM_ENCRYPT, plaintextLen, aesData->iv, AES_IV_SIZE, additionalData,
                                additionalDataLen, plaintext, output, aesData->tagLen, aesData->tag);

  if (result == 0) {
    aesData->ciphertext = output;
    aesData->ciphertextLen = plaintextLen;
    return OBC_GS_ERR_CODE_SUCCESS;
  } else if (result == MBEDTLS_ERR_GCM_BAD_INPUT) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  } else {
    return OBC_GS_ERR_CODE_ENC_FAIL;
  }
}

obc_gs_error_code_t aes128Decrypt(aes_data_t *aesData, uint8_t *output, uint8_t outputBufferLen,
                                  const uint8_t *additionalData, size_t additionalDataLen) {
  if (aesData == NULL || output == NULL) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }

  if (outputBufferLen < aesData->ciphertextLen) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }

  int result = mbedtls_gcm_auth_decrypt(&gcm_ctx, aesData->ciphertextLen, aesData->iv, AES_IV_SIZE, additionalData,
                                        additionalDataLen, aesData->tag, aesData->tagLen, aesData->ciphertext, output);

  if (result == 0) {
    return OBC_GS_ERR_CODE_SUCCESS;
  } else if (result == MBEDTLS_ERR_GCM_AUTH_FAILED) {
    return OBC_GS_ERR_CODE_AUTH_FAILED;
  } else if (result == MBEDTLS_ERR_GCM_BAD_INPUT) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  } else {
    return OBC_GS_ERR_CODE_DEC_FAIL;
  }
}
