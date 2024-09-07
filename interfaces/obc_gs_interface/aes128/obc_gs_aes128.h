#pragma once

#include "obc_gs_errors.h"

#include <stdint.h>
#include <stddef.h>

#define AES_BLOCK_SIZE 16U
#define AES_KEY_SIZE 16U
#define AES_IV_SIZE 16U
#define AES_DECRYPTED_SIZE RS_DECODED_SIZE - AES_IV_SIZE

typedef struct {
  uint8_t iv[AES_IV_SIZE];
  uint8_t *ciphertext;
  size_t ciphertextLen;
} aes_data_t;

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief Decrypts the AES blocks
 *
 * @param aesData Pointer to an aes_data_t struct that includes a struct of the IV and data
 * @param output array to store the decrypted data
 * @param outputBufferLen length of the buffer to store the decrypted data
 */
obc_gs_error_code_t aes128Decrypt(aes_data_t *aesData, uint8_t *output, uint8_t outputBufferLen);

/**
 * @brief Encrypts the data into the AES block the size must be the same as the output buffer
 *
 * @param data array storing the data to be encrypted
 * @param outputBufferLen length of the buffer storing the data to be encrypted
 * @param data Pointer to an aes_data_t struct that includes a struct of the IV and data
 */
obc_gs_error_code_t aes128Encrypt(const uint8_t *data, uint8_t outputBufferLen, aes_data_t *output);

/**
 * @brief Initializes the AES context
 *
 * @param key - The key to decrypt the AES blocks with
 */
obc_gs_error_code_t initializeAesCtx(const uint8_t *key);
#ifdef __cplusplus
}
#endif
