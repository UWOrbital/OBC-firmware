#ifndef PAYLOAD_INCLUDE_OV5642_REGS_H_
#define PAYLOAD_INCLUDE_OV5642_REGS_H_

#include "stdint.h"

/**
 * @struct sensor_reg_t
 * @brief	Sensor reg struct, reg is the address to write to and val is the value to write
*/
typedef struct {
	int reg;
	int val;
} sensor_reg_t;

/**
 * @enum	cam_config_t
 * @brief	Configuration array names to be called with getCamConfig().
 *
 * Configuration array names.
*/
typedef enum {
  OV5642_QVGA_Preview_Config,
  OV5642_JPEG_Capture_QSXGA_Config,
	OV5642_320x240_Config
} cam_config_t;

/**
 * @brief Access camera configuration arrays
 * @param config  Camera config array name
 * @return Pointer to config array
 */
sensor_reg_t* getCamConfig(cam_config_t config);

#endif /* PAYLOAD_INCLUDE_OV5642_REGS_H_ */
