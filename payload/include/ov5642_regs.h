#ifndef PAYLOAD_INCLUDE_OV5642_REGS_H_
#define PAYLOAD_INCLUDE_OV5642_REGS_H_

#include "stdint.h"

/**
 * @struct sensor_reg
 * @brief	Sensor reg struct, reg is the address to write to and val is the value to write
*/
struct sensor_reg {
	int reg;
	int val;
};

extern const struct sensor_reg OV5642_QVGA_Preview[];
extern const struct sensor_reg OV5642_JPEG_Capture_QSXGA[];
extern const struct sensor_reg ov5642_320x240[];

#endif /* PAYLOAD_INCLUDE_OV5642_REGS_H_ */
