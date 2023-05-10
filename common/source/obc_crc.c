#include "obc_crc.h"
#include "obc_errors.h"

#include <stdint.h>
#include <stddef.h>

obc_error_code_t computeCrc32(const uint8_t* data, uint32_t length, uint32_t* crc) {
    if (data == NULL || crc == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    // TODO: Compute CRC32
    *crc = 0xFFFFFFFF;

    return OBC_ERR_CODE_SUCCESS;
}
