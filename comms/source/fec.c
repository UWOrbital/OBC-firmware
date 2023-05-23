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
 * @brief Decodes the reed solomon data
 * 
 * @param rsData 255 byte array that has encoded reed solomon data
 * @param decodedData pointer to a union that includes a uint8_t array of size 223B
 * 
 * @return obc_error_code_t - whether or not the data was successfully decoded
*/
obc_error_code_t rsDecode(packed_rs_packet_t *rsData, uint8_t *decodedData){
    if (rsData == NULL)
        return OBC_ERR_CODE_INVALID_ARG;

    if (decodedData == NULL)
        return OBC_ERR_CODE_INVALID_ARG;

    // TODO: Implement Reed-Solomon decoding

    return OBC_ERR_CODE_SUCCESS;
}
