#pragma once

#include "stdint.h"

#define PREVIEW_CONFIG_LEN 583
#define JPEG_CONFIG_LEN 71
#define RES_320_240_CONFIG_LEN 26

/**
 * @struct sensor_reg_t
 * @brief	Sensor reg struct, reg is the address to write to and val is the value to write
 */
typedef struct {
  uint16_t reg;
  uint8_t val;
} sensor_reg_t;

/**
 * @enum	cam_config_t
 * @brief	Configuration array names to be called with getCamConfig().
 *
 * Configuration array names.
 */
typedef enum { OV5642_QVGA_Preview_Config, OV5642_JPEG_Capture_QSXGA_Config, OV5642_320x240_Config } cam_config_t;

/**
 * @brief Access camera configuration arrays
 * @param config  Camera config array name
 * @return Pointer to config array
 */
sensor_reg_t* getCamConfig(cam_config_t config);
