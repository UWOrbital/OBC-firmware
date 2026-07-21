#pragma once
#include "arducam.h"

/**
 * @brief Sets the SPI bus and I2C mux for a specific camera
 *
 * @param camID Camera ID
 * @return Error code. OBC_ERR_CODE_SUCCESS if successful.
 */
obc_error_code_t selectCamera(camera_id_t camID);

/**
 * @brief Configures ov5642 sensor to defaults
 *
 * @return Error code. OBC_ERR_CODE_SUCCESS if successful.
 */
obc_error_code_t camConfigureSensor(void);

/**
 * @brief Selects, initializes and configures the camera, then triggers a capture and
 *        blocks until the capture completes.
 *
 * On success the JPEG lives in the Arducam's on-board FIFO and can be streamed out with
 * repeated readImage() calls - no OBC-side buffer for the whole image is needed.
 *
 * @param camID Camera to capture with
 * @return Error code. OBC_ERR_CODE_SUCCESS if the capture completed.
 */
obc_error_code_t captureImage(camera_id_t camID);
