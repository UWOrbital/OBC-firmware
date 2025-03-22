#include "arducam.h"
#include "ov5642_config.h"

#include "obc_i2c_io.h"
#include "obc_spi_io.h"
#include "obc_reliance_fs.h"
#include "obc_board_config.h"

#include <gio.h>
#include <sci.h>
#include <spi.h>
#include <i2c.h>
#include <redposix.h>
#include <string.h>

// Constants

#define ARDUCAM_DELAY_2MS pdMS_TO_TICKS(2)

// Arduchip masks
#define ARDUCAM_RESET_CPLD_MASK 0x80

// Camera Img Sensor (I2C) defines
#define CAM_I2C_ADDR 0x3C
#define I2C_MUTEX_TIMEOUT portMAX_DELAY
#define I2C_TRANSFER_TIMEOUT pdMS_TO_TICKS(100)

// SPI values
static spiDAT1_t arducamSPIDataFmt = {.CS_HOLD = 0, .CSNR = SPI_CS_NONE, .DFSEL = CAM_SPI_DATA_FORMAT, .WDEL = 0};

static const uint8_t cameraCS[CAMERA_COUNT] = {
    [PRIMARY] = CAM_CS_1,
    [SECONDARY] = CAM_CS_2,
};

static camera_id_t selectedCamera = PRIMARY;

typedef enum opcode {
  ARDUCAM_READ_TEST_REG = 0x00,
  ARDUCAM_WRITE_TEST_REG = (0x00 | 0x80),
  ARDUCAM_READ_CAPTURE_CONTROL_REG = 0x01,
  ARDUCAM_WRITE_CAPTURE_CONTROL_REG = (0x01 | 0x80),
  ARDUCAM_READ_SENSOR_TIMING_CONTROL_REG = 0x03,
  ARDUCAM_WRITE_SENSOR_TIMING_CONTROL_REG = (0x03 | 0x80),
  ARDUCAM_READ_FIFO_CONTROL_REG = 0x04,
  ARDUCAM_WRITE_FIFO_CONTROL_REG = (0x04 | 0x80),
  ARDUCAM_READ_SENSOR_POWER_CONTROL_REG = 0x06,
  ARDUCAM_WRITE_SENSOR_POWER_CONTROL_REG = (0x06 | 0x80),
  ARDUCAM_FIFO_BURST_READ = 0x3C,
  ARDUCAM_FIFO_READ = 0x3D,
  ARDUCAM_READ_FW_VERSION = 0x40,
  ARDUCAM_READ_CAPTURE_STATUS_REG = 0x41,
  ARDUCAM_READ_FIFO_SIZE_LOWER = 0x42,
  ARDUCAM_READ_FIFO_SIZE_MIDDLE = 0x43,
  ARDUCAM_READ_FIFO_SIZE_UPPER = 0x44,
  ARDUCAM_RESET_CPLD = (0x07 | 0x80),
} opcode_t;

void selectCameraSPIBus(camera_id_t cameraID) { selectedCamera = cameraID; }

camera_id_t getSelectedCamera(void) { return selectedCamera; }

// CS assumed to be asserted
static obc_error_code_t arducamTransmitOpcode(opcode_t opcode) {
  obc_error_code_t errCode;
  LOG_IF_ERROR_CODE(spiTransmitByte(CAM_SPI_REG, &arducamSPIDataFmt, opcode));
  return errCode;
}

static obc_error_code_t arducamWriteRegister(opcode_t opcode, uint8_t value) {
  obc_error_code_t errCode;
  obc_error_code_t prevCode;

  RETURN_IF_ERROR_CODE(assertChipSelect(CAM_SPI_PORT, cameraCS[selectedCamera]));
  LOG_IF_ERROR_CODE(arducamTransmitOpcode(opcode));
  if (errCode == OBC_ERR_CODE_SUCCESS) {
    LOG_IF_ERROR_CODE(spiTransmitByte(CAM_SPI_REG, &arducamSPIDataFmt, value));
  }
  prevCode = errCode;
  RETURN_IF_ERROR_CODE(deassertChipSelect(CAM_SPI_PORT, cameraCS[selectedCamera]));
  // Reset overwritten error code
  errCode = prevCode;

  return errCode;
}

static obc_error_code_t arducamReadRegister(opcode_t opcode, uint8_t* buffer) {
  obc_error_code_t errCode;
  obc_error_code_t prevCode;
  if (buffer == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  RETURN_IF_ERROR_CODE(assertChipSelect(CAM_SPI_PORT, cameraCS[selectedCamera]));
  LOG_IF_ERROR_CODE(arducamTransmitOpcode(opcode));
  if (errCode == OBC_ERR_CODE_SUCCESS) {
    LOG_IF_ERROR_CODE(spiReceiveByte(CAM_SPI_REG, &arducamSPIDataFmt, buffer));
  }
  prevCode = errCode;
  RETURN_IF_ERROR_CODE(deassertChipSelect(CAM_SPI_PORT, cameraCS[selectedCamera]));
  // Reset overwritten error code
  errCode = prevCode;

  return errCode;
}

obc_error_code_t arducamReadTestReg(uint8_t *buffer) {
  return arducamReadRegister(ARDUCAM_READ_TEST_REG, buffer);
}

obc_error_code_t arducamWriteTestReg(uint8_t value) {
  return arducamWriteRegister(ARDUCAM_WRITE_TEST_REG, value);
}

obc_error_code_t arducamReadCaptureControlReg(uint8_t *buffer) {
  return arducamReadRegister(ARDUCAM_READ_CAPTURE_CONTROL_REG, buffer);
}

obc_error_code_t arducamWriteCaptureControlReg(uint8_t value) {
  if (value > 7) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  return arducamWriteRegister(ARDUCAM_WRITE_CAPTURE_CONTROL_REG, value);
}

obc_error_code_t arducamReadSensorTimingControlReg(uint8_t *buffer) {
  return arducamReadRegister(ARDUCAM_READ_SENSOR_TIMING_CONTROL_REG, buffer);
}

obc_error_code_t arducamWriteSensorTimingControlReg(uint8_t value) {
  return arducamWriteRegister(ARDUCAM_WRITE_SENSOR_TIMING_CONTROL_REG, value);
}

obc_error_code_t arducamReadFIFOControlReg(uint8_t *buffer) {
  return arducamReadRegister(ARDUCAM_READ_FIFO_CONTROL_REG, buffer);
}

obc_error_code_t arducamWriteFIFOControlReg(uint8_t value) {
  return arducamWriteRegister(ARDUCAM_WRITE_FIFO_CONTROL_REG, value);
}

obc_error_code_t arducamReadSensorPowerControlReg(uint8_t *buffer) {
  return arducamReadRegister(ARDUCAM_READ_SENSOR_POWER_CONTROL_REG, buffer);
}

obc_error_code_t arducamWriteSensorPowerControlReg(uint8_t value) {
  return arducamWriteRegister(ARDUCAM_WRITE_SENSOR_POWER_CONTROL_REG, value);
}

obc_error_code_t arducamReadFIFO(uint8_t *buffer) {
  return arducamReadRegister(ARDUCAM_FIFO_READ, buffer);
}

obc_error_code_t arducamBurstReadFIFO(uint8_t *buffer, size_t bufferSize) {
  obc_error_code_t errCode;
  obc_error_code_t prevCode;

  if (buffer == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  RETURN_IF_ERROR_CODE(assertChipSelect(CAM_SPI_PORT, cameraCS[selectedCamera]));
  LOG_IF_ERROR_CODE(arducamTransmitOpcode(ARDUCAM_FIFO_BURST_READ));
  for (size_t index = 0; (index < bufferSize) && (errCode == OBC_ERR_CODE_SUCCESS); index++) {
    LOG_IF_ERROR_CODE(spiReceiveByte(CAM_SPI_REG, &arducamSPIDataFmt, &buffer[index]));
  }
  prevCode = errCode;
  RETURN_IF_ERROR_CODE(deassertChipSelect(CAM_SPI_PORT, cameraCS[selectedCamera]));
  // Reset overwritten error code
  errCode = prevCode;

  return errCode;
}

obc_error_code_t arducamReadFWVersion(uint8_t *version) {
  return arducamReadRegister(ARDUCAM_READ_FW_VERSION, version);
}

obc_error_code_t arducamReadCaptureStatusReg(uint8_t *status) {
  return arducamReadRegister(ARDUCAM_READ_CAPTURE_STATUS_REG, status);
}

obc_error_code_t arducamReadFIFOSize(uint32_t *fifoSize) {
  obc_error_code_t errCode;
  uint8_t upper = 0;
  uint8_t middle = 0;
  uint8_t lower = 0;
  if (fifoSize == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  *fifoSize = 0;

  // Get upper bits
  RETURN_IF_ERROR_CODE(arducamReadRegister(ARDUCAM_READ_FIFO_SIZE_UPPER, &upper));
  // Get middle bits
  RETURN_IF_ERROR_CODE(arducamReadRegister(ARDUCAM_READ_FIFO_SIZE_MIDDLE, &middle));
  // Get lower bits
  RETURN_IF_ERROR_CODE(arducamReadRegister(ARDUCAM_READ_FIFO_SIZE_LOWER, &lower));

  *fifoSize = upper;
  *fifoSize = (*fifoSize << 8) | middle;
  *fifoSize = (*fifoSize << 8) | lower;

  return errCode;
}

obc_error_code_t arducamResetCPLD(void) {
  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(arducamWriteRegister(ARDUCAM_RESET_CPLD, ARDUCAM_RESET_CPLD_MASK));
  vTaskDelay(ARDUCAM_DELAY_2MS);
  RETURN_IF_ERROR_CODE(arducamWriteRegister(ARDUCAM_RESET_CPLD, 0));
  return errCode;
}

obc_error_code_t camWriteSensorReg16_8(uint32_t regID, uint8_t regDat) {
  uint8_t reg_tx_data[3] = {(regID >> 8), (regID & 0x00FF), regDat};
  return i2cSendTo(CAM_I2C_ADDR, 3, reg_tx_data, I2C_MUTEX_TIMEOUT, I2C_TRANSFER_TIMEOUT);
}

obc_error_code_t camReadSensorReg16_8(uint32_t regID, uint8_t *regDat) {
  obc_error_code_t errCode;
  uint8_t reg_id_tx_data[2] = {(regID >> 8), (regID & 0x00FF)};
  RETURN_IF_ERROR_CODE(i2cSendTo(CAM_I2C_ADDR, 2, reg_id_tx_data, I2C_MUTEX_TIMEOUT, I2C_TRANSFER_TIMEOUT));
  RETURN_IF_ERROR_CODE(i2cReceiveFrom(CAM_I2C_ADDR, 1, regDat, I2C_MUTEX_TIMEOUT, I2C_TRANSFER_TIMEOUT));
  return errCode;
}

obc_error_code_t camWriteSensorRegs16_8(const sensor_config_t reglist[], size_t reglistLen) {
  obc_error_code_t errCode;

  for (size_t i = 0; i < reglistLen; i++) {
    RETURN_IF_ERROR_CODE(camWriteSensorReg16_8(reglist[i].reg, reglist[i].val));
  }

  return OBC_ERR_CODE_SUCCESS;
}
