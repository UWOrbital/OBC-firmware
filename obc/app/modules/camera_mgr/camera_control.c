#include "camera_control.h"
#include "obc_errors.h"
#include "obc_logging.h"

#include <FreeRTOS.h>
#include <os_task.h>

#include <spi.h>

// Poll cadence and ceiling while waiting for a capture to finish.
#define CAM_CAPTURE_POLL_DELAY_MS 5U
#define CAM_CAPTURE_MAX_POLLS 1000U  // ~5s worst case at 5ms/poll

obc_error_code_t selectCamera(camera_id_t cameraID) {
  // TODO: Validate I2C mux driver code and properly integrate with control code
  return OBC_ERR_CODE_SUCCESS;
}

// TODO: Find proper config settings and capture sequence
// Following config step is ripped straight from arduino arducam example files, unsure how
// Camera is generating images, just super under exposed, brightness super low
obc_error_code_t camConfigureSensor(void) {
  obc_error_code_t errCode;
  // Reset camera
  RETURN_IF_ERROR_CODE(ov5642Reset());
  // Setup Preview resolution
  applyCamPreviewConfig();
  vTaskDelay(pdMS_TO_TICKS(2));

  // Switch to JPEG capture
  applyCamCaptureConfig();
  // Switch to lowest JPEG resolution
  applyCamResolutionConfig();

  vTaskDelay(pdMS_TO_TICKS(1));
  // Vertical flip
  RETURN_IF_ERROR_CODE(ov5642SetVerticalFlip(true));
  // Pixel binning
  // RETURN_IF_ERROR_CODE(camWriteSensorReg16_8(0x3621, 0x10));
  // Image horizontal control
  RETURN_IF_ERROR_CODE(ov5642SetHorizontalStart(432));
  // Image compression
  RETURN_IF_ERROR_CODE(ov5642SetQuantizationScale(0x08));
  // Lens correction
  RETURN_IF_ERROR_CODE(ov5642SetLencBrvScale(0x0C));
  // Image processor setup
  RETURN_IF_ERROR_CODE(ov5642SetLencCorrection(true));

  return errCode;
}

obc_error_code_t captureImage(camera_id_t camID) {
  obc_error_code_t errCode;

  RETURN_IF_ERROR_CODE(selectCamera(camID));
  RETURN_IF_ERROR_CODE(initCamera(camID));
  RETURN_IF_ERROR_CODE(camConfigureSensor());
  RETURN_IF_ERROR_CODE(startImageCapture(camID));

  // isCaptureDone() returns OBC_ERR_CODE_CAMERA_CAPTURE_INCOMPLETE until the frame is
  // fully in the FIFO. Poll (yielding the CPU) rather than busy-spinning.
  obc_error_code_t captureStatus = OBC_ERR_CODE_CAMERA_CAPTURE_INCOMPLETE;
  for (uint32_t polls = 0; polls < CAM_CAPTURE_MAX_POLLS; polls++) {
    captureStatus = isCaptureDone(camID);
    if (captureStatus != OBC_ERR_CODE_CAMERA_CAPTURE_INCOMPLETE) {
      break;
    }
    vTaskDelay(pdMS_TO_TICKS(CAM_CAPTURE_POLL_DELAY_MS));
  }

  if (captureStatus == OBC_ERR_CODE_CAMERA_CAPTURE_COMPLETE) {
    return OBC_ERR_CODE_SUCCESS;
  }

  // Either a real error from isCaptureDone(), or we exhausted the poll budget.
  return (captureStatus == OBC_ERR_CODE_CAMERA_CAPTURE_INCOMPLETE) ? OBC_ERR_CODE_CAMERA_CAPTURE_INCOMPLETE
                                                                   : captureStatus;
}
