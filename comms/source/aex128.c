#include "aes128.h"
#include "obc_logging.h"

#include <stdint.h>
#include <sys_common.h>

/**
 * @brief decrypts the AES blocks
 * 
 * @param aesData 128 byte AES block that needs to be decrypted
 * @param cmdBytes 128 byte array to store the decrypted data
 * 
 * @return obc_error_code_t - whether or not the data was successfully decrypted
*/
obc_error_code_t aes128Decrypt(aes_block_t *aesData, uint8_t *cmdBytes){
    if(aesData == NULL){
        return OBC_ERR_CODE_INVALID_ARG;
    }

    if(cmdBytes == NULL){
        return OBC_ERR_CODE_INVALID_ARG;
    }

    /* TODO: add implementation here */

    return OBC_ERR_CODE_SUCCESS;
}