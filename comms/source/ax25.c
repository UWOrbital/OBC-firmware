#include "ax25.h"
#include "fec.h"

obc_error_code_t ax25Frame(packed_rs_packet_t *rsData, packed_ax25_packet_t *ax25Data) {
    if (rsData) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    if (ax25Data) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    // TODO: Implement AX.25 framing

    return OBC_ERR_CODE_SUCCESS;
}

