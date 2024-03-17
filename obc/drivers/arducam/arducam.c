#include "arducam.h"
#include "ov5642_reg.h"

#include "obc_i2c_io.h"
#include "obc_spi_io.h"
#include "obc_reliance_fs.h"
#include "obc_board_config.h"

#include <gio.h>
#include <sci.h>
#include <spi.h>
#include <i2c.h>
#include <redposix.h>

// Camera FIFO Control (SPI) defines
#define ARDUCHIP_TRIG 0x41     // Trigger source
#define ARDUCHIP_TIM 0x03      // Timing control
#define VSYNC_LEVEL_MASK 0x02  // 0 = High active , 		1 = Low active
#define ARDUCHIP_FRAMES \
  0x01  // FRAME control register, Bit[2:0] = Number of frames to be captured  //  On 5MP_Plus platforms bit[2:0] = 7
        // means continuous capture until frame buffer is full
#define CAP_DONE_MASK 0x08
#define BURST_FIFO_READ 0x3C  // Burst FIFO read operation

#define ARDUCHIP_FIFO 0x04  // FIFO and I2C control
#define FIFO_CLEAR_MASK 0x01
#define FIFO_START_MASK 0x02
#define MAX_FIFO_SIZE 0x7FFFFF  // 8MByte

#define FIFO_SIZE1 0x42  // Camera write FIFO size[7:0] for burst to read
#define FIFO_SIZE2 0x43  // Camera write FIFO size[15:8]
#define FIFO_SIZE3 0x44  // Camera write FIFO size[18:16]

// Camera Img Sensor (I2C) defines
#define CAM_I2C_ADDR 0x3C

#define I2C_MUTEX_TIMEOUT portMAX_DELAY
#define I2C_TRANSFER_TIMEOUT pdMS_TO_TICKS(100)

static uint8_t m_fmt;
// Todo: support multiple image captures in different files
static const char fname[] = "image.jpg";

// SPI values
static spiDAT1_t arducamSPIDataFmt = {.CS_HOLD = 0, .CSNR = SPI_CS_NONE, .DFSEL = CAM_SPI_DATA_FORMAT, .WDEL = 0};
static cam_settings_t cam_config[] = {
    [PRIMARY] = {.cs_num = CAM_CS_1},
    [SECONDARY] = {.cs_num = CAM_CS_2},
};

void setFormat(image_format_t fmt) {
  if (fmt == BMP)
    m_fmt = BMP;
  else if (fmt == RAW)
    m_fmt = RAW;
  else
    m_fmt = JPEG;
}

obc_error_code_t initCam(void) {
  obc_error_code_t errCode;
  // Reset camera
  RETURN_IF_ERROR_CODE(camWriteSensorReg16_8(0x3008, 0x80));
  vTaskDelay(pdMS_TO_TICKS(2));
  // Setup at 320x420 resolution
  RETURN_IF_ERROR_CODE(camWriteSensorRegs16_8(getCamConfig(OV5642_QVGA_Preview_Config), PREVIEW_CONFIG_LEN));
  vTaskDelay(pdMS_TO_TICKS(1));
  if (m_fmt == JPEG) {
    vTaskDelay(pdMS_TO_TICKS(1));
    // Switch to JPEG capture
    RETURN_IF_ERROR_CODE(camWriteSensorRegs16_8(getCamConfig(OV5642_JPEG_Capture_QSXGA_Config), JPEG_CONFIG_LEN));
    // Switch to lowest JPEG resolution
    RETURN_IF_ERROR_CODE(camWriteSensorRegs16_8(getCamConfig(OV5642_320x240_Config), RES_320_240_CONFIG_LEN));

    vTaskDelay(pdMS_TO_TICKS(1));
    // Vertical flip
    RETURN_IF_ERROR_CODE(camWriteSensorReg16_8(0x3818, 0xa8));
    // Pixel binning
    RETURN_IF_ERROR_CODE(camWriteSensorReg16_8(0x3621, 0x10));
    // Image horizontal control
    RETURN_IF_ERROR_CODE(camWriteSensorReg16_8(0x3801, 0xb0));
    // Image compression
    RETURN_IF_ERROR_CODE(camWriteSensorReg16_8(0x4407, 0x04));
    // Lens correction
    RETURN_IF_ERROR_CODE(camWriteSensorReg16_8(0x5888, 0x00));
    // Image processor setup
    RETURN_IF_ERROR_CODE(camWriteSensorReg16_8(0x5000, 0xFF));
  }
  return errCode;
}

obc_error_code_t ov5642SetJpegSize(image_resolution_t size) {
  obc_error_code_t errCode;
  switch (size) {
    // Todo: all other resolutions are unimplemented
    case OV5642_320x240:
      RETURN_IF_ERROR_CODE(camWriteSensorRegs16_8(getCamConfig(OV5642_320x240_Config), RES_320_240_CONFIG_LEN));
      break;
    case OV5642_640x480:
      // camWriteSensorRegs16_8(ov5642_640x480);
      break;
    case OV5642_1024x768:
      // camWriteSensorRegs16_8(ov5642_1024x768);
      break;
    case OV5642_1280x960:
      // camWriteSensorRegs16_8(ov5642_1280x960);
      break;
    case OV5642_1600x1200:
      // camWriteSensorRegs16_8(ov5642_1600x1200);
      break;
    case OV5642_2048x1536:
      // camWriteSensorRegs16_8(ov5642_2048x1536);
      break;
    case OV5642_2592x1944:
      // camWriteSensorRegs16_8(ov5642_2592x1944);
      break;
    default:
      RETURN_IF_ERROR_CODE(camWriteSensorRegs16_8(getCamConfig(OV5642_320x240_Config), RES_320_240_CONFIG_LEN));
      break;
  }

  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t flushFifo(camera_t cam) { return camWriteReg(ARDUCHIP_FIFO, FIFO_CLEAR_MASK, cam); }

obc_error_code_t startCapture(camera_t cam) { return camWriteReg(ARDUCHIP_FIFO, FIFO_START_MASK, cam); }

obc_error_code_t clearFifoFlag(camera_t cam) { return camWriteReg(ARDUCHIP_FIFO, FIFO_CLEAR_MASK, cam); }

obc_error_code_t setFifoBurst(camera_t cam) { return camWriteByte(BURST_FIFO_READ, cam); }

obc_error_code_t captureImage(camera_t cam) {
  obc_error_code_t errCode;
  errCode = flushFifo(cam);
  if (!errCode) {
    errCode = clearFifoFlag(cam);
  }

  if (!errCode) {
    errCode = startCapture(cam);
  }

  return errCode;
}

bool isCaptureDone(camera_t cam) { return (bool)getBit(ARDUCHIP_TRIG, CAP_DONE_MASK, cam); }

obc_error_code_t readFifoLength(uint32_t *length, camera_t cam) {
  obc_error_code_t errCode;
  uint32_t len1 = 0, len2 = 0, len3 = 0;
  uint8_t rx_data = 0;

  RETURN_IF_ERROR_CODE(camReadReg(FIFO_SIZE1, &rx_data, cam));
  len1 = rx_data;
  RETURN_IF_ERROR_CODE(camReadReg(FIFO_SIZE2, &rx_data, cam));
  len2 = rx_data;
  RETURN_IF_ERROR_CODE(camReadReg(FIFO_SIZE3, &rx_data, cam));
  len3 = (rx_data & 0x7f);

  *length = ((len3 << 16) | (len2 << 8) | len1) & 0x07fffff;
  return errCode;
}

obc_error_code_t readFifoBurst(camera_t cam) {
  obc_error_code_t errCode;
  int32_t file = 0;
  uint32_t length = 0;
  uint8_t temp = 0, temp_last = 0;
  bool is_header = false;

  // Open a new image file
  RETURN_IF_ERROR_CODE(createFile(fname, &file));

  readFifoLength(&length, cam);
  if (length >= MAX_FIFO_SIZE) {
    // 512 kb
    errCode = OBC_ERR_CODE_FRAME_SIZE_OUT_OF_RANGE;
  } else if (length == 0) {
    // 0 kb
    errCode = OBC_ERR_CODE_FRAME_SIZE_OUT_OF_RANGE;
  }

  RETURN_IF_ERROR_CODE(errCode);
  RETURN_IF_ERROR_CODE(assertChipSelect(CAM_SPI_PORT, 1));

  // Set fifo to burst mode, receive continuous data until EOF
  errCode = setFifoBurst(cam);
  camReadByte(&temp, cam);
  length--;
  while (length-- && !errCode) {
    temp_last = temp;
    errCode = camReadByte(&temp, cam);
    if (!errCode) {
      if (is_header == true) {
        errCode = writeFile(file, &temp, 1);
      } else if ((temp == 0xD8) & (temp_last == 0xFF)) {
        is_header = true;
        errCode = writeFile(file, &temp_last, 1);
        if (!errCode) {
          errCode = writeFile(file, &temp, 1);
        }
      }
      if ((temp == 0xD9) && (temp_last == 0xFF)) {
        break;
      }
    }

    // Todo: Can this be changed to ~15us instead?
    vTaskDelay(pdMS_TO_TICKS(1));
  }

  if (!errCode) {
    errCode = deassertChipSelect(CAM_SPI_PORT, 1);
  } else {
    // If there was an error during capture, deassert without an error check
    deassertChipSelect(CAM_SPI_PORT, 1);
  }

  if (!errCode) {
    errCode = closeFile(file);
  }

  return errCode;
}

uint8_t getBit(uint8_t addr, uint8_t bit, camera_t cam) {
  uint8_t temp;
  camReadReg(addr, &temp, cam);
  temp = temp & bit;
  return temp;
}

obc_error_code_t camWriteReg(uint8_t addr, uint8_t data, camera_t cam) {
  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(assertChipSelect(CAM_SPI_PORT, cam_config[cam].cs_num));
  addr = addr | 0x80;
  uint8_t tx[2] = {addr, data};
  errCode = spiTransmitBytes(CAM_SPI_REG, &arducamSPIDataFmt, tx, 2);
  RETURN_IF_ERROR_CODE(deassertChipSelect(CAM_SPI_PORT, cam_config[cam].cs_num));
  return errCode;
}

obc_error_code_t camReadReg(uint8_t addr, uint8_t *rx_data, camera_t cam) {
  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(assertChipSelect(CAM_SPI_PORT, cam_config[cam].cs_num));
  addr = addr & 0x7F;
  errCode = spiTransmitByte(CAM_SPI_REG, &arducamSPIDataFmt, addr);
  if (!errCode) {
    errCode = spiReceiveByte(CAM_SPI_REG, &arducamSPIDataFmt, rx_data);
  }
  RETURN_IF_ERROR_CODE(deassertChipSelect(CAM_SPI_PORT, cam_config[cam].cs_num));
  return errCode;
}

obc_error_code_t camWriteByte(uint8_t byte, camera_t cam) {
  obc_error_code_t errCode;
  // RETURN_IF_ERROR_CODE(assertChipSelect(CAM_SPI_PORT, cam_config[cam].cs_num));
  errCode = spiTransmitByte(CAM_SPI_REG, &arducamSPIDataFmt, byte);
  // RETURN_IF_ERROR_CODE(deassertChipSelect(CAM_SPI_PORT, cam_config[cam].cs_num));
  return errCode;
}

obc_error_code_t camReadByte(uint8_t *byte, camera_t cam) {
  return spiReceiveByte(CAM_SPI_REG, &arducamSPIDataFmt, byte);
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

obc_error_code_t camWriteSensorRegs16_8(const sensor_reg_t reglist[], uint16_t reglistLen) {
  obc_error_code_t errCode;

  for (int i = 0; i < reglistLen; i++) {
    RETURN_IF_ERROR_CODE(camWriteSensorReg16_8(reglist[i].reg, reglist[i].val));
  }

  return OBC_ERR_CODE_SUCCESS;
}
