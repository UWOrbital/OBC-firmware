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

/**
 * @brief Creates a position data manager struct of size ADCS_POSITION_DATA_SIZE. Could allow the function to
 * be flexible by taking in a length but then would need to use malloc and free and track the size in the struct.
 * 
 * @return A new position data manager with no data
*/
position_data_manager_t initPositionDataManager(void);

/**
 * @brief Returns true if data1 and data2 are identifical. 
 * 
 * @param data1 The first positional data point
 * @param data2 The second positional data point
 * 
 * @return True if each corresponding value of the data1 and data2 are equal and false otherwise
*/
int equalsPositionData(const position_data_t data1, const position_data_t data2);

/**
 * @brief Gets the adjusted value at the given julian date
 * 
 * @attention Requires manager is a valid pointer; julianDate is greater or equal to the min julian date stored in manager
 * 
 * @param manager The manager from which to get the corresponding position data at the julianDate parameter
 * @param julianDate The julian date for which we want to get the data from the manager
 * 
 * @return The position data taken from the manager at given julian date
*/
position_data_t getPositionData(const position_data_manager_t *manager, float julianDate);

/**
 * @brief Gets the minimum julian date stored in manager
 * @attention Requires manager is a valid pointer
 * @param manager The manager from which the maximum julian date was taken
 * @return The minimum julian date stored in the manager
*/
float getMinJulianDate(const position_data_manager_t *manager);

/**
 * @brief Gets the maxiumum julian date stored in manager uses the write index location
 * @attention Requires manager is a valid pointer
 * @param manager The manager from which the maximum julian date was taken
 * @return The maximum julian date stored in the manager
*/
float getMaxJulianDate(const position_data_manager_t *manager);

#endif /* ADCS_INCLUDE_POSITON_DATA_H_ */