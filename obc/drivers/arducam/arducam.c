#include "arducam.h"
#include "ov5642_reg.h"
#include "obc_spi_io.h"
#include "obc_reliance_fs.h"
#include "obc_board_config.h"

#include <redposix.h>

// Camera control
#define ARDUCHIP_TRIG 0x41     // Trigger source
#define ARDUCHIP_TIM 0x03      // Timing control
#define VSYNC_LEVEL_MASK 0x02  // 0 = High active , 		1 = Low active
#define ARDUCHIP_FRAMES \
  0x01  // FRAME control register, Bit[2:0] = Number of frames to be captured  //  On 5MP_Plus platforms bit[2:0] = 7
        // means continuous capture until frame buffer is full
#define CAP_DONE_MASK 0x04
#define BURST_FIFO_READ 0x3C  // Burst FIFO read operation

#define ARDUCHIP_FIFO 0x04  // FIFO and I2C control
#define FIFO_CLEAR_MASK 0x01
#define FIFO_START_MASK 0x02
#define MAX_FIFO_SIZE 0x7FFFFF  // 8MByte

#define FIFO_SIZE1 0x42  // Camera write FIFO size[7:0] for burst to read
#define FIFO_SIZE2 0x43  // Camera write FIFO size[15:8]
#define FIFO_SIZE3 0x44  // Camera write FIFO size[18:16]

static uint8_t m_fmt;
// Todo: support multiple image captures in different files
static const char fname[] = "image.jpg";

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
    RETURN_IF_ERROR_CODE(camWriteSensorReg16_8(0x4407, 0x08));
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
    errCode = startCapture(cam);
  }
  if (!errCode) {
    errCode = clearFifoFlag(cam);
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
  len1 = rx_data;
  RETURN_IF_ERROR_CODE(camReadReg(FIFO_SIZE3, &rx_data, cam));
  len1 = (rx_data & 0x7f);

  *length = ((len3 << 16) | (len2 << 8) | len1) & 0x07fffff;
  return errCode;
}

// Todo: Not hardware tested
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
