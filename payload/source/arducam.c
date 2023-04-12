#include "arducam.h"
#include "camera_reg.h"
#include "ov5642_regs.h"

#include <spi.h>
#include <gio.h>
#include "obc_i2c_io.h"
#include "obc_spi_io.h"

static uint8_t m_fmt;

void InitCAM() {
  wrSensorReg16_8(0x3008, 0x80);
  wrSensorRegs16_8(OV5642_QVGA_Preview);
  // delay(100);
  if (m_fmt == JPEG) {
    // delay(100);
    wrSensorRegs16_8(OV5642_JPEG_Capture_QSXGA);
    wrSensorRegs16_8(ov5642_320x240);
    // delay(100);
    wrSensorReg16_8(0x3818, 0xa8);
    wrSensorReg16_8(0x3621, 0x10);
    wrSensorReg16_8(0x3801, 0xb0);
    wrSensorReg16_8(0x4407, 0x08);
    wrSensorReg16_8(0x5888, 0x00);
    wrSensorReg16_8(0x5000, 0xFF); 
  }
}

void set_format(uint8_t fmt) {
  if (fmt == BMP)
    m_fmt = BMP;
  else if(fmt == RAW)
    m_fmt = RAW;
  else
    m_fmt = JPEG;
}

void flush_fifo(){
  write_reg(ARDUCHIP_FIFO, FIFO_CLEAR_MASK);
}

void start_capture()
{
	write_reg(ARDUCHIP_FIFO, FIFO_START_MASK);
}

void clear_fifo_flag()
{
	write_reg(ARDUCHIP_FIFO, FIFO_CLEAR_MASK);
}

uint32_t read_fifo_length(void)
{
	uint32_t len1,len2,len3,length=0;
	len1 = read_reg(FIFO_SIZE1);
  len2 = read_reg(FIFO_SIZE2);
  len3 = read_reg(FIFO_SIZE3) & 0x7f;
  length = ((len3 << 16) | (len2 << 8) | len1) & 0x07fffff;
	return length;	
}

void set_fifo_burst(){
    spiTransmitByte(SPI_REG, &spi_config, BURST_FIFO_READ);
}

uint8_t read_fifo(void)
{
	uint8_t data;
	data = bus_read(SINGLE_FIFO_READ);
	return data;
}

uint8_t read_reg(uint8_t addr){
  uint8_t data;
  data = bus_read(addr & 0x7F);
  return data;
}

void write_reg(uint8_t addr, uint8_t data)
{
	bus_write(addr | 0x80, data); 
}

void set_bit(uint8_t addr, uint8_t bit)
{
	uint8_t temp;
	temp = read_reg(addr);
	write_reg(addr, temp | bit);
}

//Clear corresponding bit 
void clear_bit(uint8_t addr, uint8_t bit)
{
	uint8_t temp;
	temp = read_reg(addr);
	write_reg(addr, temp & (~bit));
}

//Get corresponding bit status
uint8_t get_bit(uint8_t addr, uint8_t bit)
{
  uint8_t temp;
  temp = read_reg(addr);
  temp = temp & bit;
  return temp;
}

uint8_t bus_write(int address,int value)
{	
	assertChipSelect(gioPORTA, 0);
	spiTransmitByte(SPI_REG, &spi_config, address);
  spiTransmitByte(SPI_REG, &spi_config, value);
	deassertChipSelect(gioPORTA, 0);
	return 1;
}

uint8_t bus_read(int address)
{
	uint8_t value;
  assertChipSelect(gioPORTA, 0);
	spiTransmitByte(SPI_REG, &spi_config, address);
  value = spiTransmitByte(SPI_REG, &spi_config, 0x00);
	// take the SS pin high to de-select the chip:
	deassertChipSelect(gioPORTA, 0);
	return value;
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
