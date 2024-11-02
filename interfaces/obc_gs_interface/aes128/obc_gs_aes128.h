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
  uint8_t *additionalData;
  size_t additionalDataLen;
  uint8_t *decryptedOutput;
  size_t decryptedOutputLen;
} aes_data_t;

/**
 * @brief Initializes the GCM context with a given encryption key.
 *
 * @param key The key to initialize the AES-GCM context with (128 bits)
 * @return obc_gs_error_code_t Returns OBC_GS_ERR_CODE_SUCCESS on successful initialization,
 *         OBC_GS_ERR_CODE_INVALID_ARG if the key is NULL, or OBC_GS_ERR_CODE_INIT_FAIL for other failures.
 */
obc_gs_error_code_t initializeAesCtx(const uint8_t *key);

/**
 * @brief Decrypts the AES blocks in GCM mode.
 *
 * This function decrypts the provided ciphertext using the AES-GCM mode, ensuring both confidentiality and
 * authenticity. The authentication tag is verified to ensure the data has not been tampered with.
 *
 * @param aesData Pointer to an aes_data_t struct containing all necessary data for decryption.
 *                The struct must include:
 *                - iv: Initialization vector used during encryption
 *                - ciphertext: Pointer to the ciphertext to be decrypted
 *                - ciphertextLen: Length of the ciphertext
 *                - tag: Authentication tag generated during encryption
 *                - tagLen: Length of the authentication tag
 *                - additionalData: Pointer to additional authenticated data (AAD), if any
 *                - additionalDataLen: Length of the AAD
 *                - decryptedOutput: Pre-allocated buffer where the decrypted plaintext will be stored
 *                - decryptedOutputLen: Size of the decryptedOutput buffer (must be >= ciphertextLen)
 *
 * @return obc_gs_error_code_t Returns OBC_GS_ERR_CODE_SUCCESS on successful decryption and authentication,
 *         OBC_GS_ERR_CODE_AUTH_FAILED if the authentication fails, and OBC_GS_ERR_CODE_INVALID_ARG for invalid input
 * parameters.
 */
obc_gs_error_code_t aes128Decrypt(aes_data_t *aesData);

/**
 * @brief Encrypts the AES blocks in GCM mode.
 *
 * This function encrypts the provided plaintext using the AES-GCM mode. It generates an authentication tag.
 *
 * @param aesData Pointer to an aes_data_t struct containing encryption parameters and where results will be stored.
 *                The following fields must be set before calling:
 *                - iv: Initialization vector
 *                - ciphertext: Pre-allocated buffer where the ciphertext will be stored
 *                - ciphertextLen: Size of the ciphertext buffer (must be >= plaintextLen)
 *                - tagLen: Desired length of the authentication tag (typically AES_TAG_SIZE)
 *                - additionalData: Pointer to additional authenticated data (AAD), if any
 *                - additionalDataLen: Length of the AAD
 *                After the call, the following fields will be filled:
 *                - ciphertext: Will contain the encrypted data
 *                - tag: Will contain the generated authentication tag
 * @param plaintext Pointer to the plaintext data to be encrypted.
 * @param plaintextLen Length of the plaintext data.
 *
 * @return obc_gs_error_code_t Returns OBC_GS_ERR_CODE_SUCCESS on successful encryption,
 *         OBC_GS_ERR_CODE_INVALID_ARG for invalid input parameters, and OBC_GS_ERR_CODE_ENC_FAIL for encryption
 * failures.
 */
obc_gs_error_code_t aes128Encrypt(aes_data_t *aesData, const uint8_t *plaintext, size_t plaintextLen);

#ifdef __cplusplus
}
#endif
