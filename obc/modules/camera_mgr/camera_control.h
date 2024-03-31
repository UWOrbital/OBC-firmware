#include "arducam.h"

/**
 * @brief Returns number of Bytes left in FIFO to read
 *
 * @return number of bytes
 */
size_t bytesLeftInFIFO(void);

/**
 * @brief Initialize selected camera.
 */
obc_error_code_t initCamera(void);

/**
 * @brief Returns status of capture.
 * @note Will return true if errors occur. Capture is done, just incorrectly.
 *
 * @return true if capture is done.
 */
bool isCaptureDone(void);

obc_error_code_t camConfigureSensor(void);
