#pragma once
#include "arducam.h"

/**
 * @brief Initialize selected camera.
 *
 * @return Error code. OBC_ERR_CODE_SUCCESS if successful.
 */
obc_error_code_t initCamera(void);

/**
 * @brief Returns status of capture.
 *
 * @return Error code. OBC_ERR_CODE_CAMERA_CAPTURE_COMPLETE if capture is complete,
 * OBC_ERR_CODE_CAMERA_CAPTURE_INCOMPLETE if capture is INCOMPLETE. Any error codes otherwise.
 */
obc_error_code_t isCaptureDone(void);

/**
 * @brief Starts image capture on selected camera.
 *
 * @return Error code. OBC_ERR_CODE_SUCCESS if successful.
 */
obc_error_code_t startImageCapture(void);

/**
 * @brief Reads bufferLen number of bytes from camera FIFO.
 *
 * @param buffer Pointer to buffer.
 * @param bufferLen Length of buffer in bytes.
 * @param bytesRead Number of bytes read.
 * @return obc_error_code_t
 */
obc_error_code_t readImage(uint8_t* buffer, size_t bufferLen, size_t* bytesRead);

/**
 * @brief Returns number of Bytes left in FIFO to read
 *
 * @return number of bytes
 */
size_t bytesLeftInFIFO(void);

/**
 * @brief Power up camera sensor our of Standby.
 *
 * @return Error code. OBC_ERR_CODE_SUCCESS if successful.
 */
obc_error_code_t wakeupCamera(void);

/**
 * @brief Power down camera sensor into standby. Sensor configuration will NOT be lost.
 *
 * @return Error code. OBC_ERR_CODE_SUCCESS if successful.
 */
obc_error_code_t standbyCamera(void);

/**
 * @brief Power on camera sensor. Sensor will have to be reconfigured.
 *
 * @return Error code. OBC_ERR_CODE_SUCCESS if successful.
 */
obc_error_code_t powerOnCamera(void);

/**
 * @brief Power off camera sensor. Sensor configuration will be lost.
 *
 * @return Error code. OBC_ERR_CODE_SUCCESS if successful.
 */
obc_error_code_t powerOffCamera(void);

/**
 * @brief Reset camera sensor configuration.
 *
 * @return obc_error_code_t
 */
obc_error_code_t resetCamera(void);

/**
 * @brief Configures ov5642 sensor
 *
 * @return Error code. OBC_ERR_CODE_SUCCESS if successful.
 */
obc_error_code_t camConfigureSensor(void);
