#include "obc_gs_aes128.h"
#include "obc_gs_errors.h"

#include <aes.h>

#include "mbedtls/gcm.h"
// docs:
// https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/gcm_8h/#gcm_8h_1af264b64b26c4720188b530cfccddb4ef

#include <stdint.h>
#include <string.h>

// Store the AES context
// static struct AES_ctx ctx;
static mbedtls_gcm_context gcm_ctx;

// /**
//  * @brief Decrypts the AES blocks
//  *
//  * @param aesData Pointer to an aes_data_t struct that includes a struct of the IV and data
//  * @param output array to store the decrypted data
//  * @param outputBufferLen length of the buffer to store the decrypted data
//  *
//  * @return obc_gs_error_code_t - whether or not the data was successfully decrypted
//  */
// obc_gs_error_code_t aes128Decrypt(aes_data_t *aesData, uint8_t *output, uint8_t outputBufferLen) {
//   if (aesData == NULL) {
//     return OBC_GS_ERR_CODE_INVALID_ARG;
//   }

//   if (output == NULL) {
//     return OBC_GS_ERR_CODE_INVALID_ARG;
//   }

//   if (outputBufferLen != aesData->ciphertextLen) {
//     return OBC_GS_ERR_CODE_INVALID_ARG;
//   }

//   memcpy(output, aesData->ciphertext, aesData->ciphertextLen);
//   AES_ctx_set_iv(&ctx, aesData->iv);
//   AES_CTR_xcrypt_buffer(&ctx, output, aesData->ciphertextLen);

//   return OBC_GS_ERR_CODE_SUCCESS;
// }

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

// Authenticated decryption using GCM
// authentication tag ensures data hasnt been tampered with
obc_gs_error_code_t gcmDecrypt(aes_data_t *aesData, uint8_t *output, uint8_t outputBufferLen, const uint8_t *tag,
                               size_t tag_len) {
  if (aesData == NULL || output == NULL || tag == NULL) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }

  // if (outputBufferLen != aesData->ciphertextLen) {
  // return OBC_GS_ERR_CODE_INVALID_ARG;
  // }

  int result = mbedtls_gcm_auth_decrypt(&gcm_ctx, aesData->ciphertextLen, aesData->iv, AES_IV_SIZE, NULL, 0, tag,
                                        tag_len, aesData->ciphertext, output);
  switch (result) {
    // no errors
    case 0:
      return OBC_GS_ERR_CODE_SUCCESS;
    // tag doesn't match
    case MBEDTLS_ERR_GCM_AUTH_FAILED:
      return OBC_GS_ERR_CODE_AUTH_FAILED;
    // if the lengths or pointers are not valid or a cipher-specific error code if the decryption failed
    case MBEDTLS_ERR_GCM_BAD_INPUT:
      return OBC_GS_ERR_CODE_INVALID_ARG;
    // add other errror cases if needed
    default:
      return OBC_GS_ERR_CODE_DEC_FAIL;
  }
}

// /**
//  * @brief Initializes the AES context
//  *
//  * @param key - The key to decrypt the AES blocks with
//  * @return obc_gs_error_code_t - whether or not the context was successfully initialized
//  */
// obc_gs_error_code_t initializeAesCtx(const uint8_t *key) {
//   if (key == NULL) {
//     return OBC_GS_ERR_CODE_INVALID_ARG;
//   }

//   AES_init_ctx(&ctx, key);

//   return OBC_GS_ERR_CODE_SUCCESS;
// }

/**
 * @brief Initializes the GCM context with a given encryption key.
 *
 * This function sets up the GCM context
 * * @param key - The key to decrypt the AES blocks with (128 bit?)
 */

// Initialize GCM context
void initializeGcmCtx(const uint8_t *key) {
  mbedtls_gcm_init(&gcm_ctx);
  mbedtls_gcm_setkey(&gcm_ctx, MBEDTLS_CIPHER_ID_AES, key, 128);  // Assuming 128-bit keys
}
