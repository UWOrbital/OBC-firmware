#ifndef CONFIG_CC1120
#include "obc_errors.h"
#include "obc_general_util.h"
#include "cc1120.h"
#include "cc1120_defs.h"
#include "cc1120_mcu.h"

obc_error_code_t cc1120ReadSpi(uint8_t addr, uint8_t data[], uint8_t len) {
  UNUSED(addr);
  UNUSED(data);
  UNUSED(len);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t cc1120ReadExtAddrSpi(uint8_t addr, uint8_t data[], uint8_t len) {
  UNUSED(addr);
  UNUSED(data);
  UNUSED(len);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t cc1120WriteSpi(uint8_t addr, const uint8_t data[], uint8_t len) {
  UNUSED(addr);
  UNUSED(data);
  UNUSED(len);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t cc1120WriteExtAddrSpi(uint8_t addr, const uint8_t data[], uint8_t len) {
  UNUSED(addr);
  UNUSED(data);
  UNUSED(len);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t cc1120StrobeSpi(uint8_t addr) {
  UNUSED(addr);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t cc1120ReadFifo(uint8_t data[], uint8_t len) {
  UNUSED(data);
  UNUSED(len);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t cc1120WriteFifo(uint8_t data[], uint8_t len) {
  UNUSED(data);
  UNUSED(len);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t cc1120ReadFifoDirect(uint8_t addr, uint8_t data[], uint8_t len) {
  UNUSED(addr);
  UNUSED(data);
  UNUSED(len);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t cc1120WriteFifoDirect(uint8_t addr, uint8_t data[], uint8_t len) {
  UNUSED(addr);
  UNUSED(data);
  UNUSED(len);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t cc1120SendByteReceiveStatus(uint8_t data) {
  UNUSED(data);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t cc1120GetBytesInTxFifo(uint8_t *numBytes) {
  UNUSED(numBytes);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t cc1120GetBytesInRxFifo(uint8_t *numBytes) {
  UNUSED(numBytes);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t cc1120GetState(cc1120_state_t *stateNum) {
  UNUSED(stateNum);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t cc1120Init(void) { return OBC_ERR_CODE_SUCCESS; }

obc_error_code_t mcuCC1120SpiTransfer(uint8_t outb, uint8_t *inb) {
  UNUSED(outb);
  UNUSED(inb);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t mcuCC1120CSAssert(void) { return OBC_ERR_CODE_SUCCESS; }

obc_error_code_t mcuCC1120CSDeassert(void) { return OBC_ERR_CODE_SUCCESS; }

#endif  // CONFIG_CC1120
