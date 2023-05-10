#include "fec.h"
#include "obc_logging.h"
#include "correct.h"
#include "aes128.h"

#include <stdint.h>
#include <sys_common.h>

/**
 * @brief takes in a packed telemtry array and encodes it using reed solomon
 * 
 * @param telemData packed telemtry data array that needs to be encoded
 * @param rsData 255 byte array with the reed solomon encoded data
 * 
 * @return obc_error_code_t - whether or not the data was successfully encoded
*/
obc_error_code_t rsEncode(packed_telem_t *telemData, packed_rs_packet_t *rsData) {
    if (telemData == NULL)
        return OBC_ERR_CODE_INVALID_ARG;

    if (rsData == NULL)
        return OBC_ERR_CODE_INVALID_ARG;

    // Create RS encryption
    correct_reed_solomon* rs = correct_reed_solomon_create(correct_rs_primitive_polynomial_ccsds, 1, 1, 32);

    correct_reed_solomon_encode(rs, telemData->data, REED_SOLOMON_DECODED_BYTES, rsData->data);

    return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief decodes the reed solomon data and splits it into 2 128B AES blocks
 * 
 * @param rsData 255 byte array that has encoded reed solomon data
 * @param aesData pointer to an array of aes_block_t structs to store the decoded aes blocks
 * 
 * @return obc_error_code_t - whether or not the data was successfully decoded
*/
obc_error_code_t rsDecode(packed_rs_packet_t *rsData, aes_block_t *aesData[]){
    if (rsData == NULL)
        return OBC_ERR_CODE_INVALID_ARG;

    if (aesData == NULL)
        return OBC_ERR_CODE_INVALID_ARG;

    // Create RS encryption
    correct_reed_solomon* rs = correct_reed_solomon_create(correct_rs_primitive_polynomial_ccsds, 1, 1, 32);

    uint8_t rsDecodedData[REED_SOLOMON_DECODED_BYTES];

    int8_t decodedLength = correct_reed_solomon_decode(rs, rsData->data, REED_SOLOMON_DECODED_BYTES, rsDecodedData);

    if(decodedLength == -1){
        return OBC_ERR_CODE_CORRUPTED_MSG;
    }

    for(uint8_t i = 0; i < ((REED_SOLOMON_DECODED_BYTES - IV_BYTES_PER_TRANSMISSION) / AES_BLOCK_SIZE); ++i){
        for(uint8_t k = 0; k < AES_BLOCK_SIZE; ++k){
            aesData[i]->data[k] = rsDecodedData[k + i*AES_BLOCK_SIZE];
        }
    }

    return OBC_ERR_CODE_SUCCESS;
}
