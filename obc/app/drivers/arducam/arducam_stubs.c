#ifndef CONFIG_ARDUCAM
#include "obc_errors.h"
#include "obc_general_util.h"
#include "arducam.h"
#include "camera_reg.h"
#include "image_processing.h"
#include "ov5642_reg.h"

void setFormat(image_format_t fmt) { UNUSED(fmt); }

obc_error_code_t initCam(void) { return OBC_ERR_CODE_SUCCESS; }

obc_error_code_t ov5642SetJpegSize(image_resolution_t size) {
  UNUSED(size);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t captureImage(camera_t cam) {
  UNUSED(cam);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t readFifoBurst(camera_t cam) {
  UNUSED(cam);
  return OBC_ERR_CODE_SUCCESS;
}

bool isCaptureDone(camera_t cam) {
  UNUSED(cam);
  return true;
}

obc_error_code_t camWriteReg(uint8_t addr, uint8_t data, camera_t cam) {
  UNUSED(addr);
  UNUSED(data);
  UNUSED(cam);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t camReadReg(uint8_t regID, uint8_t* regDat, camera_t cam) {
  UNUSED(regID);
  UNUSED(regDat);
  UNUSED(cam);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t camWriteByte(uint8_t byte, camera_t cam) {
  UNUSED(byte);
  UNUSED(cam);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t camReadByte(uint8_t* byte, camera_t cam) {
  UNUSED(byte);
  UNUSED(cam);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t camWriteSensorReg16_8(uint32_t regID, uint8_t regDat) {
  UNUSED(regID);
  UNUSED(regDat);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t camReadSensorReg16_8(uint8_t regID, uint8_t* regDat) {
  UNUSED(regID);
  UNUSED(regDat);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t camWriteSensorRegs16_8(const sensor_reg_t reglist[], uint16_t reglistLen) {
  UNUSED(reglist);
  UNUSED(reglistLen);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t tcaSelect(camera_t cam) {
  UNUSED(cam);
  return OBC_ERR_CODE_SUCCESS;
}

uint8_t getBit(uint8_t addr, uint8_t bit, camera_t cam) {
  UNUSED(addr);
  UNUSED(bit);
  UNUSED(cam);
  return 0;
}

sensor_reg_t* getCamConfig(cam_config_t config) {
  UNUSED(config);
  return NULL;
}

#endif  // CONFIG_ARDUCAM
