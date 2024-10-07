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

obc_error_code_t writeConfigLM75BD(uint8_t devAddr, uint8_t osFaultQueueSize, uint8_t osPolarity,
                                   uint8_t osOperationMode, uint8_t devOperationMode) {
  UNUSED(devAddr);
  UNUSED(osFaultQueueSize);
  UNUSED(osPolarity);
  UNUSED(osOperationMode);
  UNUSED(devOperationMode);
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
