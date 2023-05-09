
#ifndef COMMS_INCLUDE_AES128_H_
#define COMMS_INCLUDE_AES128_H_

#include "obc_errors.h"

#include <stdint.h>

#define AES_BLOCK_SIZE 16U
#define IV_BYTES_PER_TRANSMISSION 16U

typedef struct{
    uint8_t data[AES_BLOCK_SIZE];
} aes_block_t;

/**
 * @brief decrypts the AES blocks
 * 
 * @param aesData 128 byte AES block that needs to be decrypted
 * @param cmdBytes 128 byte array to store the decrypted data
 * 
 * @return obc_error_code_t - whether or not the data was successfully decrypted
*/
obc_error_code_t aes128Decrypt(aes_block_t *aesData, uint8_t *cmdBytes);

#endif /* COMMS_INCLUDE_ AES128_H_ */