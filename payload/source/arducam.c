#include "arducam.h"
#include "camera_reg.h"
#include "ov5642_regs.h"
#include "obc_spi_io.h"

static uint8_t m_fmt;

spiDAT1_t spi_config = {
    .CS_HOLD = FALSE,
    .WDEL = FALSE,
    .DFSEL = SPI_FMT_0,
    .CSNR = SPI_CS_NONE
};

void set_format(uint8_t fmt) {
  if (fmt == BMP)
    m_fmt = BMP;
  else if(fmt == RAW)
    m_fmt = RAW;
  else
    m_fmt = JPEG;
}

void InitCAM() {
  wrSensorReg16_8(0x3008, 0x80);
  wrSensorRegs16_8(OV5642_QVGA_Preview);
  for (int i = 0; i < 250; i++) { }
  if (m_fmt == JPEG) {
    for (int i = 0; i < 250; i++) { }
    wrSensorRegs16_8(OV5642_JPEG_Capture_QSXGA);
    wrSensorRegs16_8(ov5642_320x240);
    for (int i = 0; i < 250; i++) { }
    wrSensorReg16_8(0x3818, 0xa8);
    wrSensorReg16_8(0x3621, 0x10);
    wrSensorReg16_8(0x3801, 0xb0);
    wrSensorReg16_8(0x4407, 0x08);
    wrSensorReg16_8(0x5888, 0x00);
    wrSensorReg16_8(0x5000, 0xFF); 
  }
}

void OV5642_set_JPEG_size(uint8_t size)
{
  uint8_t reg_val;

  switch (size)
  {
    case OV5642_320x240:
      wrSensorRegs16_8(ov5642_320x240);
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
      wrSensorRegs16_8(ov5642_320x240);
      break;
  }
}

void flush_fifo() {
  write_reg(ARDUCHIP_FIFO, FIFO_CLEAR_MASK);
}

void start_capture() {
	write_reg(ARDUCHIP_FIFO, FIFO_START_MASK);
}

void clear_fifo_flag() {
	write_reg(ARDUCHIP_FIFO, FIFO_CLEAR_MASK);
}

void capture_image() {
  flush_fifo();
  start_capture();
  clear_fifo_flag();
}

bool is_capture_done() {
  return (bool)get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK);
}

uint32_t read_fifo_length(void) {
	uint32_t len1, len2, len3, length = 0;
  uint8_t rx_data = 0;

	read_reg(FIFO_SIZE1, &rx_data);
  len1 = rx_data;
  read_reg(FIFO_SIZE2, &rx_data);
  len1 = rx_data;
  read_reg(FIFO_SIZE3, &rx_data);
  len1 = (rx_data & 0x7f);

  length = ((len3 << 16) | (len2 << 8) | len1) & 0x07fffff;
	return length;	
}

void read_fifo_burst() {
  uint32_t length = 0;
  length = read_fifo_length();

  if (length >= MAX_FIFO_SIZE) { // 512 kb
    char buffer[50];
    uint8_t print_len = sprintf(buffer, "ACK CMD Over size. END");
    sciPrintText((unsigned char *)buffer, print_len);
    // return 0;
  } else if (length == 0 ) { // 0 kb
    char buffer[50];
    uint8_t print_len = sprintf(buffer, "ACK CMD Size is 0. END");
    sciPrintText((unsigned char *)buffer, print_len);
    // return 0;
  }

  assertChipSelect(spiREG3, 1);

  uint8_t temp = 0, temp_last = 0;
  uint8_t outb = 0;
  bool is_header = false;
  set_fifo_burst();
  spiTransmitAndReceiveByte(spiREG3, &spi_config, outb, &temp);
  length--;
  while (length--)
  {
    temp_last = temp;
    spiTransmitAndReceiveByte(spiREG3, &spi_config, outb, &temp);
    if (is_header == true)
    {
      // Write temp to buffer or SD card
    }
    else if ((temp == 0xD8) & (temp_last == 0xFF))
    {
      is_header = true;
      // END OF IMAGE
      // Write temp_last to buffer or SD card
      // Write temp to buffer or SD card
    }
    if ( (temp == 0xD9) && (temp_last == 0xFF) ) //If find the end ,break while,
    break;
    delayMicroseconds(15);
  }
  
  deassertChipSelect(spiREG3, 1);
  is_header = false;
}
