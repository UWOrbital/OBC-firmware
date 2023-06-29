#ifndef COMMS_INCLUDE_AES128_H_
#define COMMS_INCLUDE_AES128_H_

#include "obc_errors.h"

#include <stdint.h>
#include <stddef.h>

#define AES_BLOCK_SIZE 16U
#define AES_KEY_SIZE 16U
#define AES_IV_SIZE 16U
#define AES_DECRYPTED_SIZE RS_DECODED_SIZE - AES_IV_SIZE
#define RS_DECODED_SIZE 223U  // Currently included in both AES and FEC
// TODO: Fix circular includes so that this can be removed

typedef struct {
  uint8_t iv[AES_IV_SIZE];
  uint8_t *ciphertext;
  size_t ciphertextLen;
} aes_data_t;

/**
 * @brief Decrypts the AES blocks
 *
 * @param aesData Pointer to an aes_data_t struct that includes a struct of the IV and data
 * @param output array to store the decrypted data
 * @param outputBufferLen length of the buffer to store the decrypted data
 *
 * @return obc_error_code_t - whether or not the data was successfully decrypted
 */
obc_error_code_t aes128Decrypt(aes_data_t *aesData, uint8_t *output, uint8_t outputBufferLen);

/**
 * @brief Initializes the AES context
 *
 * @param key - The key to decrypt the AES blocks with
 * @return obc_error_code_t - whether or not the context was successfully initialized
 */
obc_error_code_t initializeAesCtx(const uint8_t *key);

#endif /* COMMS_INCLUDE_ AES128_H_ */
