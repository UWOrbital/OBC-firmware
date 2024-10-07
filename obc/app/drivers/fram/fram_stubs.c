#ifndef CONFIG_FRAM
#include "obc_errors.h"
#include "obc_general_util.h"
#include "fm25v20a.h"

void initFRAM(void) {}

obc_error_code_t framReadStatusReg(uint8_t *status) {
  UNUSED(*status);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t framWriteStatusReg(uint8_t status) {
  UNUSED(status);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t framFastRead(uint32_t addr, uint8_t *buffer, size_t nBytes) {
  UNUSED(addr);
  UNUSED(*buffer);
  UNUSED(nBytes);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t framRead(uint32_t addr, uint8_t *buffer, size_t nBytes) {
  UNUSED(addr);
  UNUSED(*buffer);
  UNUSED(nBytes);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t framWrite(uint32_t addr, const uint8_t *data, size_t nBytes) {
  UNUSED(addr);
  UNUSED(*data);
  UNUSED(nBytes);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t framSleep(void) { return OBC_ERR_CODE_SUCCESS; }

obc_error_code_t framWakeUp(void) { return OBC_ERR_CODE_SUCCESS; }

obc_error_code_t framReadID(uint8_t *id, size_t nBytes) {
  UNUSED(*id);
  UNUSED(nBytes);
  return OBC_ERR_CODE_SUCCESS;
}

#endif  // CONFIG_FRAM
