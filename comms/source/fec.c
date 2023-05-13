#include "fec.h"
#include "obc_logging.h"

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

    // TODO: Implement Reed-Solomon encoding

    return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief decodes the reed solomon data and splits it into 2 128B AES blocks
 * 
 * @param rsData 255 byte array that has encoded reed solomon data
 * @param aesData pointer to an array of aes_block_t structs to store the decoded aes block
 * 
 * @return obc_error_code_t - whether or not the data was successfully decoded
*/
obc_error_code_t rsDecode(packed_rs_packet_t *rsData, aes_block_t *aesData[]){
    if (rsData == NULL)
        return OBC_ERR_CODE_INVALID_ARG;

    if (aesData == NULL)
        return OBC_ERR_CODE_INVALID_ARG;

    // TODO: Implement Reed-Solomon decoding

    return OBC_ERR_CODE_SUCCESS;
}
