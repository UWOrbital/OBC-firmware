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
obc_error_code_t rsEncode(packed_telem_packet_t *telemData, packed_rs_packet_t *rsData) {
    if (telemData == NULL)
        return OBC_ERR_CODE_INVALID_ARG;

    if (rsData == NULL)
        return OBC_ERR_CODE_INVALID_ARG;

    // Create RS encryption
    correct_reed_solomon* rs = correct_reed_solomon_create(correct_rs_primitive_polynomial_ccsds, 1, 1, 32);

    correct_reed_solomon_encode(rs, telemData->data, REED_SOLOMON_DECODED_BYTES, rsData->data);

    correct_reed_solomon_destroy(rs);

    return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief decodes the reed solomon data and splits it into 2 128B AES blocks
 * 
 * @param rsData 255 byte array that has encoded reed solomon data
 * @param aesSerializedData pointer to an array of bytes to hold the decoded reed solomon data
 * @param aesSerializedDataLen length of the aesSerializedData array
 * 
 * @return obc_error_code_t - whether or not the data was successfully decoded
*/
obc_error_code_t rsDecode(packed_rs_packet_t *rsData, uint8_t *aesSerializedData, uint8_t aesSerializedDataLen){
    if (rsData == NULL)
        return OBC_ERR_CODE_INVALID_ARG;

    if (aesSerializedData == NULL)
        return OBC_ERR_CODE_INVALID_ARG;
    
    if(aesSerializedDataLen < REED_SOLOMON_DECODED_BYTES)
        return OBC_ERR_CODE_INVALID_ARG;
    // Create RS encryption
    correct_reed_solomon* rs = correct_reed_solomon_create(correct_rs_primitive_polynomial_ccsds, 1, 1, 32);

    int8_t decodedLength = correct_reed_solomon_decode(rs, rsData->data, REED_SOLOMON_DECODED_BYTES, aesSerializedData);

    correct_reed_solomon_destroy(rs);
    
    if(decodedLength == -1)
        return OBC_ERR_CODE_CORRUPTED_MSG;

    return OBC_ERR_CODE_SUCCESS;
}

