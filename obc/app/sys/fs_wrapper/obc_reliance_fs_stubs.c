#ifndef CONFIG_SDCARD
#include "obc_errors.h"
#include "obc_reliance_fs.h"
#include "obc_general_util.h"

obc_error_code_t setupFileSystem(void) { return OBC_ERR_CODE_SUCCESS; }

obc_error_code_t mkDir(const char *dirPath) {
  UNUSED(dirPath);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t createFile(const char *filePath, int32_t *fileId) {
  UNUSED(filePath);
  UNUSED(fileId);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t closeFile(int32_t fileId) {
  UNUSED(fileId);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t writeFile(int32_t fileId, const void *data, size_t dataLen) {
  UNUSED(fileId);
  UNUSED(data);
  UNUSED(dataLen);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t readFile(int32_t fileId, void *buffer, size_t bufferSize, size_t *bytesRead) {
  UNUSED(fileId);
  UNUSED(buffer);
  UNUSED(bufferSize);
  UNUSED(bytesRead);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t getFileSize(int32_t fileId, size_t *fileSize) {
  UNUSED(fileId);
  UNUSED(fileSize);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t openFile(const char *filePath, uint32_t openMode, int32_t *fileId) {
  UNUSED(filePath);
  UNUSED(openMode);
  UNUSED(fileId);
  return OBC_ERR_CODE_SUCCESS;
}

#endif  // CONFIG_SDCARD
