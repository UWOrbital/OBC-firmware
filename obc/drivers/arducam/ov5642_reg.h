#pragma once

#include "stdint.h"

#define PREVIEW_CONFIG_LEN 583
#define JPEG_CONFIG_LEN 72
#define RES_320_240_CONFIG_LEN 27

/**
 * @struct sensor_reg_t
 * @brief	Sensor reg struct, reg is the address to write to and val is the value to write
 */
typedef struct {
  uint16_t reg;
  uint8_t val;
} sensor_reg_t;
/**
 * @brief Access camera preview configuration arrays
 * @param config  Camera config array name
 * @return Pointer to config array
 */
sensor_reg_t* getCamPreviewConfig(void);

/**
 * @brief Access camera capture configuration arrays
 * @param config  Camera config array name
 * @return Pointer to config array
 */
sensor_reg_t* getCamCaptureConfig(void);
/**
 * @brief Access camera resolution configuration arrays
 * @param config  Camera config array name
 * @return Pointer to config array
 */
sensor_reg_t* getCamResolutionConfig(void);
