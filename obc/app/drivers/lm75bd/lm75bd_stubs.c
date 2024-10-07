#ifndef CONFIG_LM75BD
#include "obc_errors.h"
#include "obc_general_util.h"
#include "lm75bd.h"

obc_error_code_t lm75bdInit(lm75bd_config_t *config) {
  UNUSED(config);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t readTempLM75BD(uint8_t devAddr, float *temp) {
  UNUSED(devAddr);
  UNUSED(temp);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t readConfigLM75BD(lm75bd_config_t *config) {
  UNUSED(config);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t readThystLM75BD(uint8_t devAddr, float *hysteresisThresholdCelsius) {
  UNUSED(devAddr);
  UNUSED(hysteresisThresholdCelsius);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t writeThystLM75BD(uint8_t devAddr, float hysteresisThresholdCelsius) {
  UNUSED(devAddr);
  UNUSED(hysteresisThresholdCelsius);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t readTosLM75BD(uint8_t devAddr, float *overTempThresholdCelsius) {
  UNUSED(devAddr);
  UNUSED(overTempThresholdCelsius);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t writeTosLM75BD(uint8_t devAddr, float overTempThresholdCelsius) {
  UNUSED(devAddr);
  UNUSED(overTempThresholdCelsius);
  return OBC_ERR_CODE_SUCCESS;
}

void osHandlerLM75BD(uint8_t devAddr) { UNUSED(devAddr); }

#endif  // CONFIG_LM75BD
