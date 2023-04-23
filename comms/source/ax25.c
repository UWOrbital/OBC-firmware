#include "ax25.h"
#include "obc_logging.h"

#include <stdint.h>
#include <sys_common.h>

/**
 * @brief adds ax.25 headers onto telemtry being downlinked
 * 
 * @param rsData reed solomon data that needs ax.25 headers added onto it
 * @param out array to store the ax.25 frame
 * 
 * @return obc_error_code_t - whether or not the ax.25 headers were successfully added
*/
obc_error_code_t ax25Send(packed_rs_packet_t *rsData, packed_ax25_packet_t *ax25Data) {
    if (rsData == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    if (ax25Data == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    // TODO: Implement AX.25 framing

    return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief strips away the ax.25 headers from a received packet
 * 
 * @param ax25Data the received ax.25 frame
 * @param rsData 255 byte array to store the reed solomon encoded data without ax.25 headers
 * 
 * @return obc_error_code_t - whether or not the ax.25 headers were successfully stripped
*/
obc_error_code_t ax25Recv(packed_ax25_packet_t *ax25Data, packed_rs_packet_t *rsData){
    if (rsData == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    if (ax25Data == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    // TODO: Implement AX.25 frame stripping

    return OBC_ERR_CODE_SUCCESS;
}
