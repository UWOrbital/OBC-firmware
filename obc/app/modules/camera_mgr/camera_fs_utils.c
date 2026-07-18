#include "camera_fs_utils.h"
#include "obc_errors.h"
#include "obc_logging.h"
#include "obc_reliance_fs.h"

#include <redposix.h>

#include <FreeRTOS.h>
#include <os_semphr.h>

#include <stdint.h>
#include <stddef.h>

// Guards the image file for an entire open -> close session; see initImageFileMutex()
static SemaphoreHandle_t imageFileMutex = NULL;
static StaticSemaphore_t imageFileMutexBuffer;

void initImageFileMutex(void) {
  if (imageFileMutex == NULL) {
    imageFileMutex = xSemaphoreCreateMutexStatic(&imageFileMutexBuffer);
  }
}

obc_error_code_t mkImageDir(void) {
  obc_error_code_t errCode;

  RETURN_IF_ERROR_CODE(mkDir(IMAGE_FILE_DIRECTORY));

  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t openImageFileWrite(int32_t *imageFileId) {
  obc_error_code_t errCode;

  if (imageFileId == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  if (imageFileMutex == NULL) {
    return OBC_ERR_CODE_INVALID_STATE;
  }

  if (xSemaphoreTake(imageFileMutex, portMAX_DELAY) != pdPASS) {
    return OBC_ERR_CODE_MUTEX_TIMEOUT;
  }

  int32_t imageFile = 0;
  errCode = openFile(IMAGE_FILE_PATH, RED_O_WRONLY | RED_O_CREAT | RED_O_TRUNC, &imageFile);
  if (errCode != OBC_ERR_CODE_SUCCESS) {
    xSemaphoreGive(imageFileMutex);
    return errCode;
  }

  *imageFileId = imageFile;

  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t openImageFileRO(int32_t *imageFileId) {
  obc_error_code_t errCode;

  if (imageFileId == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  if (imageFileMutex == NULL) {
    return OBC_ERR_CODE_INVALID_STATE;
  }

  if (xSemaphoreTake(imageFileMutex, portMAX_DELAY) != pdPASS) {
    return OBC_ERR_CODE_MUTEX_TIMEOUT;
  }

  int32_t imageFile = 0;
  errCode = openFile(IMAGE_FILE_PATH, RED_O_RDONLY, &imageFile);
  if (errCode != OBC_ERR_CODE_SUCCESS) {
    xSemaphoreGive(imageFileMutex);
    return errCode;
  }

  *imageFileId = imageFile;

  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t closeImageFile(int32_t imageFileId) {
  obc_error_code_t errCode = closeFile(imageFileId);

  // Release the session mutex even if the close itself failed; the session is over either way
  if (imageFileMutex != NULL) {
    xSemaphoreGive(imageFileMutex);
  }

  return errCode;
}

obc_error_code_t writeImageToFile(int32_t imageFileId, const uint8_t *data, size_t dataLen) {
  // Assume file is open and valid
  obc_error_code_t errCode;

  if (data == NULL || dataLen == 0) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  RETURN_IF_ERROR_CODE(writeFile(imageFileId, data, dataLen));

  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t readNextImageChunkFromFile(int32_t imageFileId, uint8_t *buffer, size_t bufferSize,
                                            size_t *bytesRead) {
  // Assume file is open and valid
  obc_error_code_t errCode;

  if (buffer == NULL || bytesRead == NULL || bufferSize == 0) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  RETURN_IF_ERROR_CODE(readFile(imageFileId, buffer, bufferSize, bytesRead));

  if (*bytesRead == 0) {
    return OBC_ERR_CODE_REACHED_EOF;
  }

  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t getImageFileSize(int32_t imageFileId, size_t *fileSize) {
  obc_error_code_t errCode;

  if (fileSize == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  RETURN_IF_ERROR_CODE(getFileSize(imageFileId, fileSize));

  return OBC_ERR_CODE_SUCCESS;
}
