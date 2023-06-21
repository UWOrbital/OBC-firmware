#include "aes128.h"
#include "obc_logging.h"

#include "aes.h"
#include <stdint.h>
#include <string.h>

// Store the AES context
static struct AES_ctx ctx;

/**
 * @brief Decrypts the AES blocks
 * 
 * @param aesData Pointer to an aes_data_t struct that includes a struct of the IV and data
 * @param aesDataLen length of the aesData array
 * @param output array to store the decrypted data
 * 
 * @return obc_error_code_t - whether or not the data was successfully decrypted
*/
obc_error_code_t aes128Decrypt(aes_data_t *aesData, uint8_t aesDataLen, uint8_t *output){
    if(aesData == NULL){
        return OBC_ERR_CODE_INVALID_ARG;
    }

    if(output == NULL){
        return OBC_ERR_CODE_INVALID_ARG;
    }

    memcpy(output, aesData->rawData + AES_IV_SIZE, aesDataLen - AES_IV_SIZE);
    AES_ctx_set_iv(&ctx, aesData->rawData);
    AES_CTR_xcrypt_buffer(&ctx, output, aesDataLen - AES_IV_SIZE);
    memcpy(output, output+5, aesDataLen - AES_IV_SIZE-5);
    return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief Initializes the AES context
 * 
 * @param key - The key to decrypt the AES blocks with
 * @return obc_error_code_t - whether or not the context was successfully initialized
 */
obc_error_code_t initializeAesCtx(const uint8_t *key) {
    if(key == NULL){
        return OBC_ERR_CODE_INVALID_ARG;
    }

    AES_init_ctx(&ctx, key);

    return OBC_ERR_CODE_SUCCESS;
}
