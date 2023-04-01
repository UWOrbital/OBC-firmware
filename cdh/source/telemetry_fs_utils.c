#include "telemetry_fs_utils.h"
#include "telemetry_manager.h"
#include "obc_logging.h"
#include "obc_errors.h"
#include "obc_assert.h"

#include <redposix.h>

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

obc_error_code_t mkTelemetryDir(void) {
    int32_t ret = red_mkdir(TELEMETRY_FILE_DIRECTORY);
    if (ret != 0) {
        LOG_DEBUG("Failed to create telemetry directory: %d", red_errno);
        return OBC_ERR_CODE_MKDIR_FAILED;
    }

    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t createTelemetryFile(uint32_t telemBatchId, int32_t *telemFileId) {
    obc_error_code_t errCode;

    if (telemFileId == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    unsigned char telemFilePathBuffer[TELEMETRY_FILE_PATH_MAX_LENGTH] = {'\0'};
    RETURN_IF_ERROR_CODE(getTelemetryFilePath(telemBatchId, (char *)telemFilePathBuffer, TELEMETRY_FILE_PATH_MAX_LENGTH));

    // One of RED_O_RDONLY, RED_O_WRONLY, or RED_O_RDWR must be specified.
    // We're closing the file immediately anyways, so doesn't matter which one we use.
    int32_t telFile = red_open((const char *)telemFilePathBuffer, RED_O_RDONLY | RED_O_CREAT);
    if (telFile < 0) {
        return OBC_ERR_CODE_FAILED_FILE_OPEN;
    }

    *telemFileId = telFile;

    RETURN_IF_ERROR_CODE(closeTelemetryFile(telFile));

    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t openTelemetryFileRW(uint32_t telemBatchId, int32_t *telemFileId) {
    obc_error_code_t errCode;

    if (telemFileId == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    unsigned char telemFilePathBuffer[TELEMETRY_FILE_PATH_MAX_LENGTH] = {'\0'};
    RETURN_IF_ERROR_CODE(getTelemetryFilePath(telemBatchId, (char *)telemFilePathBuffer, TELEMETRY_FILE_PATH_MAX_LENGTH));

    // TODO: If we overflowed the batch ID, we should delete the duplicate file
    // However, don't delete the file if an overflow hasn't occurred (like if a system reset occurred).

    int32_t telFile = red_open((const char *)telemFilePathBuffer, RED_O_RDWR | RED_O_APPEND);
    if (telFile < 0) {
        return OBC_ERR_CODE_FAILED_FILE_OPEN;
    }

    *telemFileId = telFile;

    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t openTelemetryFileRO(uint32_t telemBatchId, int32_t *telemFileId) {
    obc_error_code_t errCode;

    if (telemFileId == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    unsigned char telemFilePathBuffer[TELEMETRY_FILE_PATH_MAX_LENGTH] = {'\0'};
    RETURN_IF_ERROR_CODE(getTelemetryFilePath(telemBatchId, (char *)telemFilePathBuffer, TELEMETRY_FILE_PATH_MAX_LENGTH));

    int32_t telFile = red_open((const char *)telemFilePathBuffer, RED_O_RDONLY);
    if (telFile < 0) {
        return OBC_ERR_CODE_FAILED_FILE_OPEN;
    }

    *telemFileId = telFile;

    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t closeTelemetryFile(int32_t telemFileId) {
    if (telemFileId < 0) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    int32_t ret = red_close(telemFileId);
    if (ret < 0) {
        return OBC_ERR_CODE_FAILED_FILE_CLOSE;
    }

    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t createAndOpenTelemetryFileRW(uint32_t telemBatchId, int32_t *telemFileId) {
    obc_error_code_t errCode;

    if (telemFileId == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    RETURN_IF_ERROR_CODE(createTelemetryFile(telemBatchId, telemFileId));

    RETURN_IF_ERROR_CODE(openTelemetryFileRW(telemBatchId, telemFileId));

    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t getTelemetryFilePath(uint32_t telemBatchId, char *buff, size_t buffSize) {
    if (buff == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    if (buffSize < TELEMETRY_FILE_PATH_MAX_LENGTH) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    int ret = snprintf(buff, buffSize, "%s%s%lu%s", TELEMETRY_FILE_DIRECTORY, TELEMETRY_FILE_PREFIX, telemBatchId, TELEMETRY_FILE_EXTENSION);
    if (ret < 0) {
        return OBC_ERR_CODE_INVALID_FILE_NAME;
    }

    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t writeTelemetryToFile(int32_t telFileId, telemetry_data_t telemetryData) {
    // Assume file is open and valid

    if (telFileId < 0) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    // TODO: Handle power resets during read/write (i.e partial data)
    // Idea: Add header and footer to each file, and check for them on read (investigate checksum feasability)
    int32_t ret = red_write(telFileId, &telemetryData, sizeof(telemetry_data_t));

    if(ret == sizeof(telemetry_data_t)) {
        return OBC_ERR_CODE_SUCCESS;
    }
    
    if (red_errno == RED_EFBIG) {
        // TODO: Handle this error; probably need to close the file and open a new one
        return OBC_ERR_CODE_MAX_FILE_SIZE_REACHED;
    }

    return OBC_ERR_CODE_FAILED_FILE_WRITE;
}

obc_error_code_t readNextTelemetryFromFile(int32_t telemFileId, telemetry_data_t *telemData) {
    // Assume file is open and valid
    
    if (telemData == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    if (telemFileId < 0) {
        return OBC_ERR_CODE_INVALID_ARG;
    }
    
    int32_t bytesRead = red_read(telemFileId, (void *) telemData, sizeof(telemetry_data_t));

    if (bytesRead == 0) {
        return OBC_ERR_CODE_REACHED_EOF;
    }

    // Since we only write the telemetry data struct, the number of bytes in the file
    // should be a multiple of the size of the struct. TODO: Deal with incomplete writes
    if (bytesRead != sizeof(telemetry_data_t)) {
        return OBC_ERR_CODE_FAILED_FILE_READ;
    }

    return OBC_ERR_CODE_SUCCESS;
}
