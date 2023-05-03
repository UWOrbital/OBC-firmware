#ifndef PAYLOAD_INCLUDE_ARDUCAM_H_
#define PAYLOAD_INCLUDE_ARDUCAM_H_

#include <stdint.h>
#include <stdbool.h>

#include "obc_errors.h"
#include "obc_logging.h"

#include "camera_reg.h"

/**
 * @enum	image_format_t
 * @brief	OV5642 supported image formats.
 *
 * Enum containing all supported image formats.
*/
typedef enum {
  BMP,
  JPEG,
  RAW
} image_format_t;

/**
 * @enum	image_resolution_t
 * @brief	OV5642 supported image resolutions.
 *
 * Enum containing all supported image resolutions.
*/
typedef enum {
  OV5642_320x240,
  OV5642_640x480,
  OV5642_1024x768,
  OV5642_1280x960 ,
  OV5642_1600x1200,
  OV5642_2048x1536,
  OV5642_2592x1944,
  OV5642_1920x1080,
} image_resolution_t;

/**
 * @brief Set format to JPEG, BMP, or RAW
 * @param fmt The image format to set
 */
void setFormat(image_format_t fmt);

/**
 * @brief Initialize camera selected by tcaSelect()
 */
obc_error_code_t initCam(void);

/**
 * @brief Change JPEG resolution
 * @param size The JPEG resolution to set
 */
obc_error_code_t ov5642SetJpegSize(image_resolution_t size);

/**
 * @brief Trigger an image capture
 */
obc_error_code_t captureImage(camera_t cam);

/**
 * @brief Checks if image capture has been completed
 * @return Returns true if capture is complete
 */
bool isCaptureDone(camera_t cam);

#endif /* PAYLOAD_INCLUDE_ARDUCAM_H_ */
