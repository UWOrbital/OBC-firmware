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

obc_gs_error_code_t aes128Encrypt(const aes_data_t *aesData, const uint8_t *plaintext, size_t plaintextLen) {
  if (aesData == NULL || plaintext == NULL || aesData->ciphertext == NULL) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }

  int result = mbedtls_gcm_crypt_and_tag(&gcm_ctx, MBEDTLS_GCM_ENCRYPT, plaintextLen, aesData->iv, AES_IV_SIZE,
                                         aesData->additionalData, aesData->additionalDataLen, plaintext,
                                         aesData->ciphertext, aesData->tagLen, aesData->tag);

  if (result == 0) {
    return OBC_GS_ERR_CODE_SUCCESS;
  } else if (result == MBEDTLS_ERR_GCM_BAD_INPUT) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  } else {
    return OBC_GS_ERR_CODE_ENC_FAIL;
  }
}

obc_gs_error_code_t aes128Decrypt(const aes_data_t *aesData, uint8_t *output, size_t outputSize) {
  if (aesData == NULL || output == NULL) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }

  if (outputSize < aesData->ciphertextLen) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }
  int result =
      mbedtls_gcm_auth_decrypt(&gcm_ctx, aesData->ciphertextLen, aesData->iv, AES_IV_SIZE, aesData->additionalData,
                               aesData->additionalDataLen, aesData->tag, aesData->tagLen, aesData->ciphertext, output);

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
