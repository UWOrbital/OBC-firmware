#include "obc_gs_aes128.h"
#include "obc_gs_errors.h"

#include <aes.h>

#include "mbedtls/gcm.h"
// https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/gcm_8h/#gcm_8h_1af264b64b26c4720188b530cfccddb4ef

#include <stdint.h>
#include <string.h>

// Store the AES context
static const mbedtls_gcm_context gcm_ctx;

/**
 * @brief Performs authenticated decryption using AES in GCM mode.
 *
 * This function decrypts the provided ciphertext using the AES GCM mode, ensuring both confidentiality and
 * authenticity. The authentication tag is verified to ensure the data has not been tampered with.
 *
 * @param aesData Pointer to an aes_data_t struct containing the IV, ciphertext, and its length. The structure must
 * include a valid IV and ciphertext.
 * @param output Buffer where the decrypted plaintext will be stored. Must be pre-allocated with sufficient space.
 * @param output_len Length of the output buffer. Must be large enough to hold the decrypted data
 * @param tag Buffer holding the authentication tag generated during encryption
 * @param tag_len Length of the authentication tag
 *
 * @return obc_gs_error_code_t - Returns OBC_GS_ERR_CODE_SUCCESS on successful decryption and authentication. Returns
 * OBC_GS_ERR_CODE_AUTH_FAILED if the authentication fails, and OBC_GS_ERR_CODE_INVALID_ARG for invalid input
 * parameters.
 */

// If the tag does not match the expected value (i.e., the value that would be computed from the ciphertext and the
// key), the function will return an error code indicating that authentication failed
obc_gs_error_code_t gcmDecrypt(aes_data_t *aesData, uint8_t *output, uint8_t outputBufferLen, const uint8_t *tag,
                               size_t tag_len) {
  if (aesData == NULL || output == NULL || tag == NULL) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }

  if (outputBufferLen < aesData->ciphertextLen) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }

  uint8_t result = mbedtls_gcm_auth_decrypt(&gcm_ctx, aesData->ciphertextLen, aesData->iv, AES_IV_SIZE, NULL, 0, tag,
                                            tag_len, aesData->ciphertext, output);
  switch (result) {
    case 0:
      return OBC_GS_ERR_CODE_SUCCESS;
    case MBEDTLS_ERR_GCM_AUTH_FAILED:
      return OBC_GS_ERR_CODE_AUTH_FAILED;
    case MBEDTLS_ERR_GCM_BAD_INPUT:
      return OBC_GS_ERR_CODE_INVALID_ARG;
    default:
      return OBC_GS_ERR_CODE_DEC_FAIL;
  }
}

/**
 * @brief Initializes the GCM context with a given encryption key.
 *
 * This function sets up the GCM context
 * * @param key - The key to decrypt the AES blocks with (128 bit?)
 */

void initializeGcmCtx(const uint8_t *key) {
  mbedtls_gcm_init(&gcm_ctx);
  mbedtls_gcm_setkey(&gcm_ctx, MBEDTLS_CIPHER_ID_AES, key, AES_KEY_BITS);
}
