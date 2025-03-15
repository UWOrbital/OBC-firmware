#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "obc_errors.h"
#include "obc_logging.h"

#include "obc_spi_io.h"
#include "ov5642_config.h"
#include "obc_board_config.h"

// Arduchip Constants
#define MAX_FIFO_SIZE 0x7FFFFF  // 8MByte

// Sensor Timing Control Masks
#define HSYNC_ACTIVE_LOW_MASK (0x01U << 0)
#define VSYNC_ACTIVE_LOW_MASK (0x01U << 1)
#define PCLK_REVERSED_MASK (0x01U << 3)
// FIFO Control Masks
#define FIFO_CLEAR_CAPTURE_DONE_FLAG (0x01U << 0)
#define FIFO_START_CAPTURE (0x01U << 1)
#define FIFO_RESET_WRITE_PTR (0x01U << 4)
#define FIFO_RESET_READ_PTR (0x01U << 5)
// Sensor Control Masks
#define SENSOR_RESET_MASK (0x01U << 0)
#define SENSOR_STANDBY_MASK (0x01 << 1)
#define SENSOR_POWER_EN_MASK (0x01U << 2)
// Capture Status Masks
#define STATUS_VSYNC_MASK (0x01U << 0)
#define STATUS_CAPTURE_DONE_MASK (0x01 << 3)

/**
 * @enum	camera_t
 * @brief	Primary or secondary camera.
 *
 * Enum containing camera identifiers and camera count.
 */
typedef enum {
  PRIMARY,
  SECONDARY,
  CAMERA_COUNT,
} camera_id_t;

/**
 * @brief Selects one of the camera.
 *
 * @param cameraID Camera ID of the camera to be selected.
 */
void selectCameraSPIBus(camera_id_t cameraID);

/**
 * @brief Returns the camera id of the selected camera.
 *
 * @return Camera ID of the selected camera.
 */
camera_id_t getSelectedCamera(void);

/**
 * @brief Resets arducam chip. Needed for some ARM architecture for SPI to work. IDK why this is not in the docs >:(
 *        https://docs.arducam.com/Arduino-SPI-camera/Legacy-SPI-camera/FAQ/
 *
 * @return Error code. OBC_ERR_CODE_SUCCESS if successful.
 */

obc_error_code_t arducamResetCPLD(void);

/**
 * @brief Read Arducam's Test Register
 *
 * @param buffer 1 byte buffer to store value read.
 * @return Error code. OBC_ERR_CODE_SUCCESS if successful.
 */
obc_error_code_t arducamReadTestReg(uint8_t* buffer);

/**
 * @brief Write a value to Arducam's Test Register
 *
 * @param value 1 byte value to be written.
 * @return Error code. OBC_ERR_CODE_SUCCESS if successful.
 */
obc_error_code_t arducamWriteTestReg(uint8_t value);

/**
 * @brief Read Arducam's Capture Control Register
 *        Value determines number of frames captured.
 *        0 -> 6 = 1 -> 7 frames captured; 7 = keep capturing
 *        until FIFO is full.
 * @param buffer 1 byte buffer to store value read.
 * @return Error code. OBC_ERR_CODE_SUCCESS if successful.
 */
obc_error_code_t arducamReadCaptureControlReg(uint8_t* buffer);

/**
 * @brief Write a value to Arducam's Capture Control Register
 *        Value determines number of frames captured.
 *        0 -> 6 = 1 -> 7 frames captured; 7 = keep capturing
 *        until FIFO is full.
 * @param value 1 byte value to be written.
 * @return Error code. OBC_ERR_CODE_SUCCESS if successful.
 */
obc_error_code_t arducamWriteCaptureControlReg(uint8_t value);

/**
 * @brief Read Arducam's Sensor Timing Control Register
 *        Bit[0] Hsync Polarity: 0 = Active High, 1 = Active Low;
 *        Bit[1] Vsync Polarity: 0 = Active High, 1 = Active Low;
 *        Bit[3] Sensor PCLK reverse: 0 = normal, 1 = reversed PCLK
 * @param buffer 1 byte buffer to store value read.
 * @return Error code. OBC_ERR_CODE_SUCCESS if successful.
 */
obc_error_code_t arducamReadSensorTimingControlReg(uint8_t* buffer);

/**
 * @brief Write a value to Arducam's Sensor Timing Control Register
 *        Bit[0] Hsync Polarity: 0 = Active High, 1 = Active Low;
 *        Bit[1] Vsync Polarity: 0 = Active High, 1 = Active Low;
 *        Bit[3] Sensor PCLK reverse: 0 = normal, 1 = reversed PCLK
 * @param value 1 byte value to be written.
 * @return Error code. OBC_ERR_CODE_SUCCESS if successful.
 */
obc_error_code_t arducamWriteSensorTimingControlReg(uint8_t value);

/**
 * @brief Read Arducam's FIFO Control Register
 *        Bit[0]: clear FIFO write/capture done flag;
 *        Bit[1]: start capture;
 *        Bit[4]: reset FIFO write pointer;
 *        Bit[5]: reset FIFO read pointer
 * @param buffer 1 byte buffer to store value read.
 * @return Error code. OBC_ERR_CODE_SUCCESS if successful.
 */
obc_error_code_t arducamReadFIFOControlReg(uint8_t* buffer);

/**
 * @brief Write a value to Arducam's FIFO Control Register
 *        Write 1 to following bits to
 *        Bit[0]: clear FIFO write/capture done flag;
 *        Bit[1]: start capture;
 *        Bit[4]: reset FIFO write pointer;
 *        Bit[5]: reset FIFO read pointer
 * @param value 1 byte value to be written.
 * @return Error code. OBC_ERR_CODE_SUCCESS if successful.
 */
obc_error_code_t arducamWriteFIFOControlReg(uint8_t value);

/**
 * @brief Read Arducam's Sensor Power Control Register
 *        Bit[0]: Reset Sensor?
 *        Bit[1]: Standby Sensor 0 = out of standby, 1 = in standby;
 *        Bit[2]: Power Down Sensor 0 = disable power, 1 = enable power;
 *        Note: After Power Down, Sensor will need to be reinitialized
 * @param buffer 1 byte buffer to store value read.
 * @return Error code. OBC_ERR_CODE_SUCCESS if successful.
 * @return obc_error_code_t
 */
obc_error_code_t arducamReadSensorPowerControlReg(uint8_t* buffer);

/**
 * @brief Write a value to Arducam's Sensor Power Control Register
 *        Bit[0]: Reset Sensor: 0 = reset sensor, 1 = out of reset;
 *        Bit[1]: Standby Sensor 0 = out of standby, 1 = in standby;
 *        Bit[2]: Power Down Sensor 0 = disable power, 1 = enable power;
 *        Note: After Power Down, Sensor will need to be reinitialized
 * @param value 1 byte value to be written.
 * @return Error code. OBC_ERR_CODE_SUCCESS if successful.
 */
obc_error_code_t arducamWriteSensorPowerControlReg(uint8_t value);

/**
 * @brief Read a byte from the Arducam FIFO
 *
 * @param buffer 1 byte buffer to store value read.
 * @return Error code. OBC_ERR_CODE_SUCCESS if successful.
 */
obc_error_code_t arducamReadFIFO(uint8_t* buffer);

/**
 * @brief Read bufferSize bytes from the Arducam FIFO
 *
 * @param buffer bufferSize byte buffer to store value read.
 * @param bufferSize size of buffer.
 * @return Error code. OBC_ERR_CODE_SUCCESS if successful.
 */
obc_error_code_t arducamBurstReadFIFO(uint8_t* buffer, size_t bufferSize);

/**
 * @brief Read Arducam's arduchip firmware verion.
 *        Interpreted as Bits[7:4] . Bits[3:0]
 * @param version 1 byte buffer to store value read.
 * @return Error code. OBC_ERR_CODE_SUCCESS if successful.
 */
obc_error_code_t arducamReadFWVersion(uint8_t* version);

/**
 * @brief Read Arducam's capture status.
 *        Bit[0] : vsync pin realtime status;
 *        Bit[3] : capture done flag
 * @param version 1 byte buffer to store value read.
 * @return Error code. OBC_ERR_CODE_SUCCESS if successful.
 */
obc_error_code_t arducamReadCaptureStatusReg(uint8_t* status);

/**
 * @brief Read the size of the Write FIFO
 *
 * @param fifoSize uint32_t buffer to store value
 * @return Error code. OBC_ERR_CODE_SUCCESS if successful.
 */
obc_error_code_t arducamReadFIFOSize(uint32_t* fifoSize);

/**
 * @brief Read 8 bits from a 16 bit register over I2C
 * @param regID Register address to write to
 * @param regDat Data to send
 * @return Error code indicating if the write was successful
 */
obc_error_code_t camWriteSensorReg16_8(uint32_t regID, uint8_t regDat);

/**
 * @brief Write 8 bits to a 16 bit register over I2C
 * @param regID Register address to read from
 * @param regDat Buffer to store received data
 * @return Error code indicating if the read was successful
 */
obc_error_code_t camReadSensorReg16_8(uint32_t regID, uint8_t* regDat);

/**
 * @brief Write to a list of registers over I2C
 * @param reglist List of registers and data to write
 * @return Error code indicating if the writes were successful
 */
obc_error_code_t camWriteSensorRegs16_8(const sensor_config_t reglist[], size_t reglistLen);
