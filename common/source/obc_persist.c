#include "obc_persist.h"
#include "obc_errors.h"
#include "obc_logging.h"
#include "obc_crc.h"
#include "fram.h"

#include <stdint.h>
#include <string.h>

obc_error_code_t getPersistTimekeeper(fram_data_timekeeper_t *persist) {
    obc_error_code_t errCode;

    if (persist == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    uint32_t persistAddr = OBC_PERSIST_BASE_ADDR + offsetof(fram_persistent_data_t, timekeeperPersist);

    uint8_t buff[sizeof(fram_persist_timekeeper_t)];
    RETURN_IF_ERROR_CODE(framRead(persistAddr, buff, sizeof(fram_persist_timekeeper_t)));

    fram_persist_timekeeper_t tk;
    memcpy(&tk, buff, sizeof(fram_persist_timekeeper_t));

    if (tk.header.len != sizeof(fram_persist_timekeeper_t)) {
        LOG_ERROR("Timekeeper length mismatch");
        return OBC_ERR_CODE_PERSIST_LENGTH_CHECK_FAILED;
    }

    uint32_t crc;
    RETURN_IF_ERROR_CODE(computeCrc32((uint8_t*)&tk.data, sizeof(fram_data_timekeeper_t), &crc));

    if (tk.header.crc32 != crc) {
        LOG_ERROR("Timekeeper data CRC mismatch");
        return OBC_ERR_CODE_PERSIST_CRC_CHECK_FAILED;
    }

    memcpy(persist, &tk.data, sizeof(fram_data_timekeeper_t));

    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t setPersistTimekeeper(const fram_data_timekeeper_t *persist) {
    obc_error_code_t errCode;

    if (persist == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    uint32_t persistAddr = OBC_PERSIST_BASE_ADDR + offsetof(fram_persistent_data_t, timekeeperPersist);

    uint32_t crc32;
    RETURN_IF_ERROR_CODE(computeCrc32((uint8_t*)persist, sizeof(fram_data_timekeeper_t), &crc32));

    fram_persist_timekeeper_t tk = {
        .header = {
            .len = sizeof(fram_persist_timekeeper_t),
            .crc32 = crc32
        },
        .data = *persist
    };

    RETURN_IF_ERROR_CODE(framWrite(persistAddr, (uint8_t*)&tk, sizeof(fram_persist_timekeeper_t)));

    return OBC_ERR_CODE_SUCCESS;
}
