#ifndef OV5642_REGS_H
#define OV5642_REGS_H

#include "stdint.h"

struct sensor_reg {
	uint16_t reg;
	uint16_t val;
};

extern const struct sensor_reg OV5642_QVGA_Preview[];
extern const struct sensor_reg OV5642_JPEG_Capture_QSXGA[];
extern const struct sensor_reg ov5642_320x240[];

#endif // OV5642_REGS_H
