#include "fec.h"
#include "send_telemetry.h"

obc_error_code_t rsEncode(packed_telem_t *telemData, packed_rs_packet_t *rsData) {
    if (telemData)
        return OBC_ERR_CODE_INVALID_ARG;

    if (rsData)
        return OBC_ERR_CODE_INVALID_ARG;

    // TODO: Implement Reed-Solomon encoding

    return OBC_ERR_CODE_SUCCESS;
}
