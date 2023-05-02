#include "arducam.h"
#include "camera_reg.h"
#include "ov5642_regs.h"
#include "obc_spi_io.h"
#include "obc_fs_utils.h"
#include "obc_board_config.h"

#include <redposix.h>

// Camera control
#define ARDUCHIP_TRIG      		0x41  // Trigger source
#define ARDUCHIP_TIM       		0x03  // Timing control
#define VSYNC_LEVEL_MASK   		0x02  // 0 = High active , 		1 = Low active
#define ARDUCHIP_FRAMES			  0x01  // FRAME control register, Bit[2:0] = Number of frames to be captured  //  On 5MP_Plus platforms bit[2:0] = 7 means continuous capture until frame buffer is full
#define CAP_DONE_MASK      		0x04
#define BURST_FIFO_READ			  0x3C  //Burst FIFO read operation

#define ARDUCHIP_FIFO      		0x04  // FIFO and I2C control
#define FIFO_CLEAR_MASK    		0x01
#define FIFO_START_MASK    		0x02
#define MAX_FIFO_SIZE		      0x7FFFFF  // 8MByte

#define FIFO_SIZE1				0x42  // Camera write FIFO size[7:0] for burst to read
#define FIFO_SIZE2				0x43  // Camera write FIFO size[15:8]
#define FIFO_SIZE3				0x44  // Camera write FIFO size[18:16]

static uint8_t m_fmt;
// Todo: support multiple image captures in different files
static const char fname[] = "/captures/imageX";

void setFormat(image_format_t fmt) {
  if (fmt == BMP)
    m_fmt = BMP;
  else if(fmt == RAW)
    m_fmt = RAW;
  else
    m_fmt = JPEG;
}

obc_error_code_t initCam(void) {
  obc_error_code_t errCode;
  // Reset camera
  RETURN_IF_ERROR_CODE(wrSensorReg16_8(0x3008, 0x80));
  // Setup at 320x420 resolution
  RETURN_IF_ERROR_CODE(wrSensorRegs16_8(OV5642_QVGA_Preview));
  vTaskDelay(pdMS_TO_TICKS(1));
  if (m_fmt == JPEG) {
    vTaskDelay(pdMS_TO_TICKS(1));
    // Switch to JPEG capture
    RETURN_IF_ERROR_CODE(wrSensorRegs16_8(OV5642_JPEG_Capture_QSXGA));
    // Switch to lowest JPEG resolution
    RETURN_IF_ERROR_CODE(wrSensorRegs16_8(ov5642_320x240));

    vTaskDelay(pdMS_TO_TICKS(1));
    // Vertical flip
    RETURN_IF_ERROR_CODE(wrSensorReg16_8(0x3818, 0xa8));
    // Pixel binning
    RETURN_IF_ERROR_CODE(wrSensorReg16_8(0x3621, 0x10));
    // Image horizontal control
    RETURN_IF_ERROR_CODE(wrSensorReg16_8(0x3801, 0xb0));
    // Image compression
    RETURN_IF_ERROR_CODE(wrSensorReg16_8(0x4407, 0x08));
    // Lens correction
    RETURN_IF_ERROR_CODE(wrSensorReg16_8(0x5888, 0x00));
    // Image processor setup
    RETURN_IF_ERROR_CODE(wrSensorReg16_8(0x5000, 0xFF)); 
  }
  return errCode;
}

obc_error_code_t ov5642SetJpegSize(image_resolution_t size)
{
  obc_error_code_t errCode;
  switch (size)
  {
    // Todo: all other resolutions are unimplemented
    case OV5642_320x240:
      errCode = wrSensorRegs16_8(ov5642_320x240);
      break;
    case OV5642_640x480:
      // wrSensorRegs16_8(ov5642_640x480);
      break;
    case OV5642_1024x768:
      // wrSensorRegs16_8(ov5642_1024x768);
      break;
    case OV5642_1280x960:
      // wrSensorRegs16_8(ov5642_1280x960);
      break;
    case OV5642_1600x1200:
      // wrSensorRegs16_8(ov5642_1600x1200);
      break;
    case OV5642_2048x1536:
      // wrSensorRegs16_8(ov5642_2048x1536);
      break;
    case OV5642_2592x1944:
      // wrSensorRegs16_8(ov5642_2592x1944);
      break;
    default:
      errCode = wrSensorRegs16_8(ov5642_320x240);
      break;
  }
  return errCode;
}

obc_error_code_t flush_fifo(void) {
  return writeReg(ARDUCHIP_FIFO, FIFO_CLEAR_MASK);
}

obc_error_code_t start_capture(void) {
	return writeReg(ARDUCHIP_FIFO, FIFO_START_MASK);
}

obc_error_code_t clear_fifo_flag(void) {
	return writeReg(ARDUCHIP_FIFO, FIFO_CLEAR_MASK);
}

obc_error_code_t set_fifo_burst(void){
  return spiTransmitByte(CAM_SPI_REG, &spi_config, BURST_FIFO_READ);
}

obc_error_code_t captureImage(void) {
  obc_error_code_t errCode;
  errCode = flush_fifo();
  if(!errCode) {
   errCode = start_capture(); 
  }
  if(!errCode) {
    errCode = clear_fifo_flag();
  }
  return errCode;
}

bool isCaptureDone(void) {
  return (bool)getBit(ARDUCHIP_TRIG, CAP_DONE_MASK);
}

obc_error_code_t read_fifo_length(uint32_t length) {
  obc_error_code_t errCode;
	uint32_t len1, len2, len3 = 0;
  uint16_t rx_data = 0;

	RETURN_IF_ERROR_CODE(readReg(FIFO_SIZE1, &rx_data));
  len1 = rx_data;
  RETURN_IF_ERROR_CODE(readReg(FIFO_SIZE2, &rx_data));
  len1 = rx_data;
  RETURN_IF_ERROR_CODE(readReg(FIFO_SIZE3, &rx_data));
  len1 = (rx_data & 0x7f);

  length = ((len3 << 16) | (len2 << 8) | len1) & 0x07fffff;
}

// Todo: Not hardware tested
obc_error_code_t read_fifo_burst(void) {
  obc_error_code_t errCode;
  int32_t file = 0;
  uint32_t length = 0;
  uint8_t temp = 0, temp_last = 0;
  bool is_header = false;

  // Open a new image file  
  RETURN_IF_ERROR_CODE(createFile(fname, &file));

  read_fifo_length(&length);
  if (length >= MAX_FIFO_SIZE) {
    // 512 kb
    errCode = OBC_ERR_CODE_FRAME_SIZE_OUT_OF_RANGE;
  } else if (length == 0 ) {
    // 0 kb
    errCode = OBC_ERR_CODE_FRAME_SIZE_OUT_OF_RANGE;
  }

  RETURN_IF_ERROR_CODE(errCode);
  RETURN_IF_ERROR_CODE(assertChipSelect(CAM_SPI_PORT, 1));

  // Set fifo to burst mode, receive continuous data until EOF
  RETURN_IF_ERROR_CODE(set_fifo_burst());
  spiReceiveByte(CAM_SPI_REG, &spi_config, &temp);
  length--;
  while(length-- && !errCode) {
    temp_last = temp;
    errCode = spiReceiveByte(CAM_SPI_REG, &spi_config, &temp);
    if(!errCode) {

      if(is_header == true) {
        RETURN_IF_ERROR_CODE(writeFile(file, &temp, 1));
      }
      else if((temp == 0xD8) & (temp_last == 0xFF)) {
        is_header = true;
        RETURN_IF_ERROR_CODE(writeFile(file, &temp_last, 1));
        RETURN_IF_ERROR_CODE(writeFile(file, &temp, 1));
      }
      if((temp == 0xD9) && (temp_last == 0xFF)) {
        break;
      }
    }

    // Todo: Can this be changed to ~15us instead?
    vTaskDelay(pdMS_TO_TICKS(1));
  }
  
  RETURN_IF_ERROR_CODE(closeFile(file));

  if(!errCode) {
    errCode = deassertChipSelect(CAM_SPI_PORT, 1);
  }
  
  return errCode;
}
