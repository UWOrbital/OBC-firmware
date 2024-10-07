#ifndef CONFIG_VN100
#include "obc_errors.h"
#include "obc_general_util.h"
#include "vn100.h"
#include "vn100_binary_parsing.h"

void initVn100(void) {}

obc_error_code_t vn100ResetModule(void) { return OBC_ERR_CODE_SUCCESS; }

obc_error_code_t vn100SetBaudrate(uint32_t baudrate) {
  UNUSED(baudrate);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t vn100SetOutputRate(uint32_t outputRateHz) {
  UNUSED(outputRateHz);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t vn100PauseAsync(void) { return OBC_ERR_CODE_SUCCESS; }

obc_error_code_t vn100ResumeAsync(void) { return OBC_ERR_CODE_SUCCESS; }

obc_error_code_t vn100StartBinaryOutputs(void) { return OBC_ERR_CODE_SUCCESS; }

obc_error_code_t vn100StopBinaryOutputs(void) { return OBC_ERR_CODE_SUCCESS; }

obc_error_code_t vn100ReadBinaryOutputs(vn100_binary_packet_t* parsedPacket) {
  UNUSED(parsedPacket);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t vn100ParsePacket(const unsigned char* packet, size_t packetLen, vn100_binary_packet_t* parsedPacket) {
  UNUSED(packet);
  UNUSED(packetLen);
  UNUSED(parsedPacket);
  return OBC_ERR_CODE_SUCCESS;
}

#endif  // CONFIG_VN100
