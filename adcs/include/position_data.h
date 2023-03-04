#ifndef ADCS_INCLUDE_POSITON_DATA_H_
#define ADCS_INCLUDE_POSITON_DATA_H_

// #include "obc_errors.h"

// #include <sys_common.h>
#include <stddef.h>

#define ADCS_POSITION_DATA_SIZE 10U
#define ADCS_INVALID_JULIAN_DATE 0

/**
 * @struct	position_data_t
 * @brief	position data struct
 * Holds a single position data point
*/
typedef struct position_data_t {
    float julianDate;
    float x;
    float y;
    float z;
} position_data_t;

/**
 * @struct	position_data_manager_t
 * @brief	position data manager struct
 *
 * Holds the position data and read/write indices. 
 * read/write indices are the next indices to read from or write to
*/
typedef struct position_data_manager_t {
    size_t readIndex;
    size_t writeIndex;
    position_data_t data[ADCS_POSITION_DATA_SIZE];
} position_data_manager_t;

#endif /* ADCS_INCLUDE_POSITON_DATA_H_ */