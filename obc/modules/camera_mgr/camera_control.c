#include "camera_control.h"

static uint32_t totalBytesToRead[CAMERA_COUNT] = {0};
static uint32_t FIFOReadPtr[CAMERA_COUNT] = {0};

size_t bytesLeftInFIFO(void) {
  camera_id_t selectedCamera = getSelectedCamera();
  return (totalBytesToRead[selectedCamera] < FIFOReadPtr[selectedCamera])
             ? 0
             : totalBytesToRead[selectedCamera] - FIFOReadPtr[selectedCamera];
}

obc_error_code_t initCamera(void) {
  obc_error_code_t errCode;
  memset(totalBytesToRead, 0, sizeof(size_t) * CAMERA_COUNT);
  memset(FIFOReadPtr, 0, sizeof(size_t) * CAMERA_COUNT);
  RETURN_IF_ERROR_CODE(resetCPLD());
  RETURN_IF_ERROR_CODE(arducamWriteSensorTimingControlReg(VSYNC_ACTIVE_LOW_MASK));
  return OBC_ERR_CODE_SUCCESS;
}

bool isCaptureDone(void) {
  obc_error_code_t errCode;
  uint8_t status;
  LOG_IF_ERROR_CODE(arducamReadCaptureStatusReg(&status));
  return (errCode == OBC_ERR_CODE_SUCCESS) ? (bool)(status & STATUS_CAPTURE_DONE_MASK) : true;
}

obc_error_code_t camConfigureSensor(void) {
  obc_error_code_t errCode;
  // Reset camera
  RETURN_IF_ERROR_CODE(camWriteSensorReg16_8(0x3008, 0x80));
  // Setup at 320x420 resolution
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
