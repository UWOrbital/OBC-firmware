#ifndef ADCS_INCLUDE_POSITON_DATA_H_
#define ADCS_INCLUDE_POSITON_DATA_H_

#include "obc_errors.h"

// #include <sys_common.h>
#include <stddef.h>

#define ADCS_POSITION_DATA_SIZE 10U

/**
 *  @struct	position_data
 * @brief	position data struct
 * Holds a single position data point
*/
typedef struct {
    float julian_date;
    float x;
    float y;
    float z;
} position_data;

/**
 * @struct	position_data_manager
 * @brief	position data manager struct
 *
 * Holds the position data and read/write indices. 
 * read/write indices are the next indices to read from or write to
*/
typedef struct {
    size_t read_index;
    size_t write_index;
    position_data data[ADCS_POSITION_DATA_SIZE];
} position_data_manager;

#endif /* ADCS_INCLUDE_POSITON_DATA_H_ */