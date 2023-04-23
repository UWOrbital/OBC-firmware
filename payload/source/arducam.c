#include "arducam.h"
#include "camera_reg.h"
#include "ov5642_regs.h"
#include "obc_spi_io.h"

#include <redposix.h>

static uint8_t m_fmt;
static const char fname[] = "/captures/imageX";

void set_format(uint8_t fmt) {
  if (fmt == BMP)
    m_fmt = BMP;
  else if(fmt == RAW)
    m_fmt = RAW;
  else
    m_fmt = JPEG;
}

obc_error_code_t InitCAM(void) {
  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(wrSensorReg16_8(0x3007, 0x80));
  RETURN_IF_ERROR_CODE(wrSensorReg16_8(0x3008, 0x80));
  RETURN_IF_ERROR_CODE(wrSensorRegs16_8(OV5642_QVGA_Preview));
  vTaskDelay(pdMS_TO_TICKS(1));
  if (m_fmt == JPEG) {
    vTaskDelay(pdMS_TO_TICKS(1));
    RETURN_IF_ERROR_CODE(wrSensorRegs16_8(OV5642_JPEG_Capture_QSXGA));
    RETURN_IF_ERROR_CODE(wrSensorRegs16_8(ov5642_320x240));
    vTaskDelay(pdMS_TO_TICKS(1));
    RETURN_IF_ERROR_CODE(wrSensorReg16_8(0x3818, 0xa8));
    RETURN_IF_ERROR_CODE(wrSensorReg16_8(0x3621, 0x10));
    RETURN_IF_ERROR_CODE(wrSensorReg16_8(0x3801, 0xb0));
    RETURN_IF_ERROR_CODE(wrSensorReg16_8(0x4407, 0x08));
    RETURN_IF_ERROR_CODE(wrSensorReg16_8(0x5888, 0x00));
    RETURN_IF_ERROR_CODE(wrSensorReg16_8(0x5000, 0xFF)); 
  }
  return errCode;
}

obc_error_code_t OV5642_set_JPEG_size(uint8_t size)
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
  return write_reg(ARDUCHIP_FIFO, FIFO_CLEAR_MASK);
}

obc_error_code_t start_capture(void) {
	return write_reg(ARDUCHIP_FIFO, FIFO_START_MASK);
}

obc_error_code_t clear_fifo_flag(void) {
	return write_reg(ARDUCHIP_FIFO, FIFO_CLEAR_MASK);
}

obc_error_code_t set_fifo_burst(void){
  return spiTransmitByte(SPI_REG, &spi_config, BURST_FIFO_READ);
}

obc_error_code_t capture_image(void) {
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

bool is_capture_done(void) {
  return (bool)get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK);
}

obc_error_code_t read_fifo_length(uint32_t length) {
  obc_error_code_t errCode;
	uint32_t len1, len2, len3 = 0;
  uint16_t rx_data = 0;

	RETURN_IF_ERROR_CODE(read_reg(FIFO_SIZE1, &rx_data));
  len1 = rx_data;
  RETURN_IF_ERROR_CODE(read_reg(FIFO_SIZE2, &rx_data));
  len1 = rx_data;
  RETURN_IF_ERROR_CODE(read_reg(FIFO_SIZE3, &rx_data));
  len1 = (rx_data & 0x7f);

  length = ((len3 << 16) | (len2 << 8) | len1) & 0x07fffff;
}

// Todo: Not hardware tested
obc_error_code_t read_fifo_burst(void) {
  obc_error_code_t errCode;
  uint32_t length = 0;
  uint8_t temp = 0, temp_last = 0;
  bool is_header = false;

  // Open new image file
  int32_t file = red_open(fname, RED_O_WRONLY | RED_O_CREAT);

  read_fifo_length(&length);
  if (length >= MAX_FIFO_SIZE) {
    // 512 kb
    errCode = OBC_ERR_CODE_FRAME_SIZE_OUT_OF_RANGE;
  } else if (length == 0 ) {
    // 0 kb
    errCode = OBC_ERR_CODE_FRAME_SIZE_OUT_OF_RANGE;
  }

  RETURN_IF_ERROR_CODE(errCode);
  RETURN_IF_ERROR_CODE(assertChipSelect(SPI_PORT, 1));

  // Set fifo to burst mode, receive continuous data until EOF
  set_fifo_burst();
  spiReceiveByte(SPI_REG, &spi_config, &temp);
  length--;
  while (length-- && errCode) {
    temp_last = temp;
    errCode = spiReceiveByte(SPI_REG, &spi_config, &temp);
    if (is_header == true) {
      // Write data to file
      red_write(file, temp, 1);
    }
    else if ((temp == 0xD8) & (temp_last == 0xFF)) {
      is_header = true;
      red_write(file, temp_last, 1);
      red_write(file, temp, 1);
    }
    if ( (temp == 0xD9) && (temp_last == 0xFF) ) {
      break;
    }
    // Todo: Can this be changed to ~15us instead?
    vTaskDelay(pdMS_TO_TICKS(1));
  }
  
  if(!errCode) {
    errCode = deassertChipSelect(SPI_PORT, 1);
  }
  return errCode;
}
