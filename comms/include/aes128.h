#ifndef COMMS_INCLUDE_AES128_H_
#define COMMS_INCLUDE_AES128_H_

#include "obc_errors.h"

#include <stdint.h>
#include <stddef.h>

#define AES_BLOCK_SIZE 16U
#define AES_KEY_SIZE 16U
#define AES_IV_SIZE 16U
#define RS_DECODED_SIZE 223U // Currently included in both AES and FEC
// TODO: Fix circular includes so that this can be removed

typedef struct {
    uint8_t iv[AES_IV_SIZE];
    uint8_t ciphertext[RS_DECODED_SIZE-AES_IV_SIZE];
} aes_struct_t;

typedef union {
    aes_struct_t aesStruct;
    uint8_t rawData[RS_DECODED_SIZE];
} aes_data_t;

/**
 * @brief Decrypts the AES blocks
 * 
 * @param aesData Pointer to an aes_data_t union that includes a struct of the IV and data
 * @param cmdBytes 128 byte array to store the decrypted data
 * 
 * @return obc_error_code_t - whether or not the data was successfully decrypted
*/
obc_error_code_t aes128Decrypt(aes_data_t *aesData, uint8_t *cmdBytes);

/**
 * @brief Initializes the AES context
 * 
 * @param key - The key to decrypt the AES blocks with
 * @return obc_error_code_t - whether or not the context was successfully initialized
 */
obc_error_code_t initializeAesCtx(const uint8_t *key);

#endif /* COMMS_INCLUDE_ AES128_H_ */