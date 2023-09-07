#pragma once

#include "data_point.h"
#include <stdint.h>

#define ADCS_POSITION_DATA_MANAGER_SIZE 10U
#define ADCS_INVALID_JULIAN_DATE 0U

// uint8_t are used as the data manager will never exceed 256 data points as that is nearly 3 kb of data
/**
 * @struct	position_data_manager_t
 * @brief	Holds the position data and read/write indices. 
 * Read/write indices are the next indices to read from or write to
*/
typedef struct position_data_manager_t {
    uint8_t readIndex;  // Will not exceed writeIndex
    uint8_t writeIndex;
    position_data_t data[ADCS_POSITION_DATA_MANAGER_SIZE];
} position_data_manager_t;

/**
 * @brief	Initializes the sun position module
*/
void initSunPosition(void);

