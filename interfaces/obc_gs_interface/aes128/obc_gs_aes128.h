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

/**
 * @brief Structure containing data for AES-GCM encryption and decryption operations
 */
typedef struct {
  /** Initialization vector used during encryption (16 bytes) */
  uint8_t iv[AES_IV_SIZE];

  /** Pointer to the ciphertext to be decrypted or buffer to store encrypted data */
  uint8_t *ciphertext;

  /** Length of the ciphertext data */
  size_t ciphertextLen;

  /** Authentication tag generated during encryption (16 bytes) */
  uint8_t tag[AES_TAG_SIZE];

  /** Length of the authentication tag (typically AES_TAG_SIZE) */
  size_t tagLen;

  /** Pointer to additional authenticated data (AAD), if any */
  uint8_t *additionalData;

  /** Length of the additional authenticated data */
  size_t additionalDataLen;
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
 * @param output Pointer to a buffer where the decrypted data will be stored.
 * @param outputSize Size of the output buffer, indicating the maximum number of bytes that can be written.
 *
 * @return obc_gs_error_code_t Returns OBC_GS_ERR_CODE_SUCCESS on successful decryption and authentication,
 *         OBC_GS_ERR_CODE_AUTH_FAILED if the authentication fails, and OBC_GS_ERR_CODE_INVALID_ARG for invalid input
 * parameters.
 */
obc_gs_error_code_t aes128Decrypt(const aes_data_t *aesData, uint8_t *output, size_t outputSize);

/**
 * @brief Encrypts the AES blocks in GCM mode.
 *
 * This function encrypts the provided plaintext using the AES-GCM mode. It generates an authentication tag.
 *
 * @param aesData Pointer to an aes_data_t struct containing encryption parameters and where results will be stored.
 * @param plaintext Pointer to the plaintext data to be encrypted.
 * @param plaintextLen Length of the plaintext data.
 *
 * @return obc_gs_error_code_t Returns OBC_GS_ERR_CODE_SUCCESS on successful encryption,
 *         OBC_GS_ERR_CODE_INVALID_ARG for invalid input parameters, and OBC_GS_ERR_CODE_ENC_FAIL for encryption
 * failures.
 */
obc_gs_error_code_t aes128Encrypt(const aes_data_t *aesData, const uint8_t *plaintext, size_t plaintextLen);

#ifdef __cplusplus
}
#endif
