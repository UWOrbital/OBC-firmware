#include "arducam.h"
#include "camera_reg.h"
#include "ov5642_regs.h"

static uint8_t m_fmt;

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
