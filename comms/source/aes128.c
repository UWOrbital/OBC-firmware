#include "aes128.h"
#include "obc_logging.h"

#include "aes.h"
#include <stdint.h>
#include <sys_common.h>

// Store the AES context
static struct AES_ctx ctx;

/**
 * @brief Decrypts the AES blocks
 * 
 * @param aesData Pointer to an aes_data_t union that includes a struct of the IV and data
 * @param cmdBytes 223B-AES_IV_SIZE array to store the decrypted data
 * 
 * @return obc_error_code_t - whether or not the data was successfully decrypted
*/
obc_error_code_t aes128Decrypt(aes_data_t *aesData, uint8_t *cmdBytes){
    if(aesData == NULL){
        return OBC_ERR_CODE_INVALID_ARG;
    }

    if(cmdBytes == NULL){
        return OBC_ERR_CODE_INVALID_ARG;
    }

    memcpy(cmdBytes, aesData->aesStruct.ciphertext, RS_DECODED_SIZE-AES_IV_SIZE);
    AES_ctx_set_iv(&ctx, aesData->aesStruct.iv);
    AES_CTR_xcrypt_buffer(&ctx, cmdBytes, RS_DECODED_SIZE-AES_IV_SIZE);

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
