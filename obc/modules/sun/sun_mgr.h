// This file is included by ephermeris.h and should NOT be included directly, unless for testing purposes
// All functions are NOT atomic. It is the responsibility of the caller to setup the appropriate locks
#pragma once

#include "sun_utils.h"

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ADCS_POSITION_DATA_MANAGER_SIZE 25U

// uint8_t are used as the data manager will never exceed 256 data points as that is nearly 5 kb of data
typedef uint8_t manager_size_t;

/**
 * @struct	position_data_manager_t
 * @brief	Holds the position data and read/write indices.
 * Read/write indices are the next indices to read from or write to
 */
typedef struct position_data_manager_t {
  manager_size_t readIndex;  // Will not exceed writeIndex
  manager_size_t writeIndex;
  position_data_t data[ADCS_POSITION_DATA_MANAGER_SIZE];
  uint8_t state;
} position_data_manager_t;

// State has the following bit expression:
//  state: 8 7 6 5 4 3 2 1
//  state: 0 0 0 0 0 0 R W
//  W indicates the write bit will fail
//  R indicates the read bit will fail

/**
 * @brief	Initializes the position data manager
 * @param	manager: The manager to initialize
 * @attention Requires that the manager is not NULL
 * @warning This operation is NOT atomic. It is the responsibility of the caller to setup the appropriate locks
 */
obc_error_code_t sunManagerInit(position_data_manager_t *manager);

/**
 * @brief	Checks that the JD is within the range of the manager
 * @warning This operation is NOT atomic. It is the responsibility of the caller to setup the appropriate locks
 * @param	manager: The manager to check
 * @param	jd: The julian date to check
 * @param buffer: Stores the boolean value of whether the JD is within the range of the manager
 * @attention Requires that the manager and buffer are not NULL
 */
obc_error_code_t sunManagerCheckJD(const position_data_manager_t *manager, julian_date_t jd, bool *buffer);

/**
 * @brief Gets the adjusted value at the given julian date
 * @attention Requires buffer and manager are valid pointers; julianDate is greater or equal to the min julian date
 * stored in manager;
 * @param manager The manager from which to get the corresponding position data at the julianDate parameter
 * @param buffer The buffer to store the position data in
 * @param julianDate The julian date for which we want to get the data from the manager
 * @warning This operation is NOT atomic. It is the responsibility of the caller to setup the appropriate locks
 */
obc_error_code_t sunManagerGetPositionData(const position_data_manager_t *manager, julian_date_t julianDate,
                                           position_data_t *buffer);

/**
 * @brief Writes the data to the manager at writeIndex and shifts the writeIndex over
 * @attention Manager must be a valid pointer; JD of data must be greater than the JD of the last data point in the
 * manager
 * @param manager The manager to which to write the data
 * @param data The data to be written to the manager
 * @warning This operation is NOT atomic. It is the responsibility of the caller to setup the appropriate locks
 *
 */
obc_error_code_t sunManagerWriteData(position_data_manager_t *manager, position_data_t data);

/**
 * @brief Reads the data from the manager into the buffer at readIndex and shifts the readIndex over
 * @attention Buffer and manager must be valid pointers
 * @param manager The manager from which to read the data
 * @param buffer The buffer to store the data in
 * @warning This operation is NOT atomic. It is the responsibility of the caller to setup the appropriate locks
 */
obc_error_code_t sunManagerReadData(position_data_manager_t *manager, position_data_t *buffer);

#ifdef __cplusplus
}
#endif
