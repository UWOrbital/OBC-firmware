#pragma once
#include "obc_gs_errors.h"
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define AES_BLOCK_SIZE 16U
#define AES_KEY_SIZE 16U
#define AES_IV_SIZE 16U
#define AES_DECRYPTED_SIZE RS_DECODED_SIZE - AES_IV_SIZE
#define AES_KEY_BITS 128U
#define AES_TAG_SIZE 16

typedef struct {
  uint8_t iv[AES_IV_SIZE];
  uint8_t *ciphertext;
  size_t ciphertextLen;
  uint8_t tag[AES_TAG_SIZE];
  size_t tagLen;
} aes_data_t;

/**
 * @brief Initializes the GCM context with a given encryption key.
 *
 * * @param key - The key to decrypt the AES blocks with (128 bit?)
 */
obc_gs_error_code_t initializeAesCtx(const uint8_t *key);

/**
 * @brief  Decrypts the AES blocks in GCM mode.
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
obc_gs_error_code_t aes128Decrypt(aes_data_t *aesData, uint8_t *output, uint8_t outputBufferLen,
                                  const uint8_t *additionalData, size_t additionalDataLen);

/**
 * @brief Encrypts the AES blocks in GCM mode.
 *
 * This function encrypts the provided plaintext using the AES GCM mode. It generates an authentication tag.
 *
 * @param plaintext Pointer to the plaintext data.
 * @param plaintextLen Length of the plaintext data.
 * @param iv Initialization vector.
 * @param iv_len Length of the initialization vector.
 * @param additionalData Additional authenticated data (AAD).
 * @param additionalDataLen Length of the additional authenticated data.
 * @param output Buffer where the ciphertext will be stored. Must be pre-allocated with sufficient space.
 * @param tag Buffer where the authentication tag will be stored.
 * @param tag_len Length of the authentication tag buffer.
 *
 * @return obc_gs_error_code_t - Returns OBC_GS_ERR_CODE_SUCCESS on successful encryption. Returns
 * OBC_GS_ERR_CODE_INVALID_ARG for invalid input parameters, and OBC_GS_ERR_CODE_ENC_FAIL for encryption failures.
 */
obc_gs_error_code_t gcmEncrypt(aes_data_t *aesData, const uint8_t *plaintext, size_t plaintextLen,
                               const uint8_t *additionalData, size_t additionalDataLen, uint8_t *output);
#ifdef __cplusplus
}
#endif
