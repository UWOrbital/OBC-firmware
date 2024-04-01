#include "camera_control.h"

static uint32_t totalBytesToRead[CAMERA_COUNT] = {0};
static uint32_t FIFOReadPtr[CAMERA_COUNT] = {0};

obc_error_code_t initCamera(void) {
  obc_error_code_t errCode;
  memset(totalBytesToRead, 0, sizeof(size_t) * CAMERA_COUNT);
  memset(FIFOReadPtr, 0, sizeof(size_t) * CAMERA_COUNT);
  RETURN_IF_ERROR_CODE(resetCPLD());
  // Make sure sensor is powered and reset is not asserted (reset is active low).
  RETURN_IF_ERROR_CODE(arducamWriteSensorPowerControlReg(SENSOR_POWER_EN_MASK | SENSOR_RESET_MASK));
  // Make sure sensor vsync timing is set to active low
  RETURN_IF_ERROR_CODE(arducamWriteSensorTimingControlReg(VSYNC_ACTIVE_LOW_MASK));
  return errCode;
}

bool isCaptureDone(void) {
  obc_error_code_t errCode;
  uint8_t status;
  LOG_IF_ERROR_CODE(arducamReadCaptureStatusReg(&status));
  return (errCode == OBC_ERR_CODE_SUCCESS) ? (bool)(status & STATUS_CAPTURE_DONE_MASK) : true;
}

obc_error_code_t startImageCapture(void) {
  obc_error_code_t errCode;
  camera_id_t selectedCamera = getSelectedCamera();
  RETURN_IF_ERROR_CODE(arducamWriteFIFOControlReg(FIFO_CLEAR_CAPTURE_DONE_FLAG | FIFO_START_CAPTURE));
  totalBytesToRead[selectedCamera] = 0;
  FIFOReadPtr[selectedCamera] = 0;
  return errCode;
}

obc_error_code_t readImage(uint8_t* buffer, size_t bufferLen, size_t* bytesRead) {
  if (buffer == NULL || bytesRead == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  obc_error_code_t errCode;
  camera_id_t selectedCamera = getSelectedCamera();
  // Start of a new image, get image length
  if (totalBytesToRead[selectedCamera] == 0) {
    RETURN_IF_ERROR_CODE(arducamReadFIFOSize(&totalBytesToRead[selectedCamera]));
  }
  size_t bytesToRead = bytesLeftInFIFO();
  if (bytesToRead > bufferLen) {
    bytesToRead = bufferLen;
  }

  *bytesRead = 0;
  RETURN_IF_ERROR_CODE(arducamBurstReadFIFO(buffer, bytesToRead));
  *bytesRead = bytesToRead;
  FIFOReadPtr[selectedCamera] += bytesToRead;
  if (bytesLeftInFIFO() > 0) {
    errCode = OBC_ERR_CODE_CAMERA_IMAGE_READ_INCOMPLETE;
  }
  return errCode;
}

size_t bytesLeftInFIFO(void) {
  camera_id_t selectedCamera = getSelectedCamera();
  return (totalBytesToRead[selectedCamera] < FIFOReadPtr[selectedCamera])
             ? 0
             : totalBytesToRead[selectedCamera] - FIFOReadPtr[selectedCamera];
}

obc_error_code_t wakeupCamera(void) {
  obc_error_code_t errCode;
  uint8_t registerVal = 0;
  RETURN_IF_ERROR_CODE(arducamReadSensorPowerControlReg(&registerVal));
  // Unset standby bit (active high)
  registerVal &= (~SENSOR_STANDBY_MASK);
  RETURN_IF_ERROR_CODE(arducamWriteSensorPowerControlReg(registerVal));
  return errCode;
}

obc_error_code_t standbyCamera(void) {
  obc_error_code_t errCode;
  uint8_t registerVal = 0;
  RETURN_IF_ERROR_CODE(arducamReadSensorPowerControlReg(&registerVal));
  // Set standby bit (active high)
  registerVal |= (SENSOR_STANDBY_MASK);
  RETURN_IF_ERROR_CODE(arducamWriteSensorPowerControlReg(registerVal));
  return errCode;
}

obc_error_code_t powerOnCamera(void) {
  obc_error_code_t errCode;
  uint8_t registerVal = 0;
  RETURN_IF_ERROR_CODE(arducamReadSensorPowerControlReg(&registerVal));
  // Set power en bit (active high)
  registerVal |= (SENSOR_POWER_EN_MASK);
  RETURN_IF_ERROR_CODE(arducamWriteSensorPowerControlReg(registerVal));
  return errCode;
}

obc_error_code_t powerOffCamera(void) {
  obc_error_code_t errCode;
  uint8_t registerVal = 0;
  RETURN_IF_ERROR_CODE(arducamReadSensorPowerControlReg(&registerVal));
  // Unset power en bit (active high)
  registerVal &= (~SENSOR_POWER_EN_MASK);
  RETURN_IF_ERROR_CODE(arducamWriteSensorPowerControlReg(registerVal));
  return errCode;
}

obc_error_code_t resetCamera(void) {
  obc_error_code_t errCode;
  uint8_t registerVal = 0;
  RETURN_IF_ERROR_CODE(arducamReadSensorPowerControlReg(&registerVal));
  // Unset reset bit (active low)
  registerVal &= (~SENSOR_RESET_MASK);
  RETURN_IF_ERROR_CODE(arducamWriteSensorPowerControlReg(registerVal));
  // Set reset bit
  registerVal |= SENSOR_RESET_MASK;
  RETURN_IF_ERROR_CODE(arducamWriteSensorPowerControlReg(registerVal));
  return errCode;
}

obc_error_code_t camConfigureSensor(void) {
  obc_error_code_t errCode;
  // Reset camera
  RETURN_IF_ERROR_CODE(camWriteSensorReg16_8(0x3008, 0x80));
  // Setup Preview resolution
  RETURN_IF_ERROR_CODE(camWriteSensorRegs16_8(getCamPreviewConfig(), PREVIEW_CONFIG_LEN));
  vTaskDelay(pdMS_TO_TICKS(1));

  vTaskDelay(pdMS_TO_TICKS(1));
  // Switch to JPEG capture
  RETURN_IF_ERROR_CODE(camWriteSensorRegs16_8(getCamCaptureConfig(), JPEG_CONFIG_LEN));
  // Switch to lowest JPEG resolution
  RETURN_IF_ERROR_CODE(camWriteSensorRegs16_8(getCamResolutionConfig(), RES_320_240_CONFIG_LEN));

  vTaskDelay(pdMS_TO_TICKS(1));
  // Vertical flip
  RETURN_IF_ERROR_CODE(camWriteSensorReg16_8(0x3818, 0xa8));
  // Pixel binning
  RETURN_IF_ERROR_CODE(camWriteSensorReg16_8(0x3621, 0x10));
  // Image horizontal control
  RETURN_IF_ERROR_CODE(camWriteSensorReg16_8(0x3801, 0xb0));
  // Image compression
  RETURN_IF_ERROR_CODE(camWriteSensorReg16_8(0x4407, 0x08));
  // Lens correction
  RETURN_IF_ERROR_CODE(camWriteSensorReg16_8(0x5888, 0x00));
  // Image processor setup
  RETURN_IF_ERROR_CODE(camWriteSensorReg16_8(0x5000, 0xFF));

  return errCode;
}
