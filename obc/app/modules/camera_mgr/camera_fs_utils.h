#pragma once

#include "obc_errors.h"

#include <stdint.h>
#include <stddef.h>

/* Image file path config */
#define IMAGE_FILE_DIRECTORY "/arducam/"
#define IMAGE_FILE_PATH IMAGE_FILE_DIRECTORY "image.jpg"

/**
 * @brief Initialize the mutex guarding access to the image file. Must be called
 *        once before the scheduler starts (i.e. from a task init function).
 *
 * The image file is written by the payload manager task and read by the downlink
 * encoder task. The mutex is held for an entire open -> close session (taken by
 * the open functions, released by closeImageFile), so a capture can never
 * truncate the file while a downlink is mid-read, and vice versa.
 */
void initImageFileMutex(void);

/**
 * @brief Create the image directory.
 *
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if successful, otherwise error code
 */
obc_error_code_t mkImageDir(void);

/**
 * @brief Open the image file for writing. The file is created if it does not
 *        exist and truncated if it does, so stale data from a previous
 *        (possibly larger) image is never left behind.
 *
 * @param imageFileId Buffer to store the image file descriptor
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if successful, otherwise error code
 * @note Blocks until the image file mutex is available; the mutex is released by closeImageFile
 */
obc_error_code_t openImageFileWrite(int32_t *imageFileId);

/**
 * @brief Open the image file in read-only mode.
 *
 * @param imageFileId Buffer to store the image file descriptor
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if successful, otherwise error code
 * @note Blocks until the image file mutex is available; the mutex is released by closeImageFile
 */
obc_error_code_t openImageFileRO(int32_t *imageFileId);

/**
 * @brief Close the image file and release the image file mutex.
 *
 * @param imageFileId File descriptor given by Reliance Edge
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if successful, otherwise error code
 */
obc_error_code_t closeImageFile(int32_t imageFileId);

/**
 * @brief Write a chunk of image data to the image file.
 *
 * @param imageFileId File descriptor given by Reliance Edge
 * @param data Image data to write to file
 * @param dataLen Length of the data in bytes
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if successful, otherwise error code
 * @note File must already be opened for writing
 */
obc_error_code_t writeImageToFile(int32_t imageFileId, const uint8_t *data, size_t dataLen);

/**
 * @brief Read the next chunk of image data from the image file.
 *
 * @param imageFileId File descriptor given by Reliance Edge
 * @param buffer Buffer to store the read data
 * @param bufferSize Size of the buffer in bytes
 * @param bytesRead Buffer to store the number of bytes read; the last chunk of
 *                  the file may be smaller than bufferSize
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if successful,
 *         OBC_ERR_CODE_REACHED_EOF if there is no data left to read, otherwise error code
 * @note File must already be opened for reading
 */
obc_error_code_t readNextImageChunkFromFile(int32_t imageFileId, uint8_t *buffer, size_t bufferSize, size_t *bytesRead);

/**
 * @brief Get the size of the image file in bytes.
 *
 * @param imageFileId File descriptor given by Reliance Edge
 * @param fileSize Buffer to store the file size
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if successful, otherwise error code
 */
obc_error_code_t getImageFileSize(int32_t imageFileId, size_t *fileSize);
