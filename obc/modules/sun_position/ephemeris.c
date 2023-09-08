#include "ephermeris.h"

#include <os_semphr.h>
#include <FreeRTOS.h>
#include <string.h>

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

// Only 1 task can access the manager at a time
static SemaphoreHandle_t managerMutex = NULL;
static StaticSemaphore_t managerBuffer;

// Only 1 task can access the file at a time
static SemaphoreHandle_t fileMutex = NULL;
static StaticSemaphore_t fileBuffer;

static uint8_t initialized = 0;  // There's probably a better way to do this
static position_data_manager_t manager;

void initSunPosition(void) {
    if (managerMutex == NULL) {
        managerMutex = xSemaphoreCreateBinaryStatic(&managerBuffer);
    }

    ASSERT(managerMutex != NULL);

    if (fileMutex == NULL) {
        fileMutex = xSemaphoreCreateBinaryStatic(&fileBuffer);
    }

    ASSERT(fileMutex != NULL);

    if (!initialized) {
        manager.readIndex = 0;
        manager.writeIndex = 0;

        for (uint8_t i = 0; i < ADCS_POSITION_DATA_MANAGER_SIZE; i++) {
            position_data_t data = {ADCS_INVALID_JULIAN_DATE, 0, 0, 0};
            manager.data[i] = data;
        }

        initialized = 1;
    }
}   

/**
 * @brief Stores the data position at the given index % ADCS_POSITION_DATA_SIZE of the manager
 * @attention Requires index is non-zero and buffer is a valid pointer and the module must be initialized
*/
static obc_error_code_t managerGetPositionDataByIndex(int index, position_data_t *buffer) {
    if (!initialized) {
        return OBC_ERR_CODE_SUN_POSITION_NOT_INITIALIZED;
    }

    if (buffer == NULL || index < 0) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    *buffer = manager.data[index % ADCS_POSITION_DATA_MANAGER_SIZE];
    return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief Stores the julian date into the buffer at the given index in the manager
 * @attention Requires buffer is a valid pointer and index >= 0 and the module must be initialized
*/
static obc_error_code_t managerGetJulianDateByIndex(int index, julian_date_t *buffer) {
    if (!initialized) {
        return OBC_ERR_CODE_SUN_POSITION_NOT_INITIALIZED;
    }

    if (index < 0 || buffer == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    position_data_t tmpData;
    obc_error_code_t errCode = managerGetPositionDataByIndex(index, &tmpData);

    if (errCode != OBC_ERR_CODE_SUCCESS) {
        return errCode;
    }

    *buffer = tmpData.julianDate;
    return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief Stores the minimum julian date into the buffer from the manager
 * @attention Requires buffer is a valid pointer and the module must be initialized
*/
static obc_error_code_t managerGetMinJulianDate(julian_date_t *buffer) {
    if (!initialized) {
        return OBC_ERR_CODE_SUN_POSITION_NOT_INITIALIZED;
    }

    if (buffer == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    *buffer = manager.data[manager.writeIndex].julianDate;
    return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief Stores the maximum julian date into the buffer from the manager
 * @attention Requires buffer is a valid pointer and the module must be initialized
*/
static obc_error_code_t managerGetMaxJulianDate(julian_date_t *buffer) {
    if (!initialized) {
        return OBC_ERR_CODE_SUN_POSITION_NOT_INITIALIZED;
    }

    if (buffer == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    uint8_t index = (manager.writeIndex - 1 + ADCS_POSITION_DATA_MANAGER_SIZE) % ADCS_POSITION_DATA_MANAGER_SIZE;
    *buffer = manager.data[index].julianDate;
    return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief Reads the data from the manager into the buffer at readIndex and shifts the readIndex over
 * @attention buffer must be a valid pointer and the module must be initialized
*/
static obc_error_code_t managerReadData(position_data_t *buffer) {
    if (!initialized) {
        return OBC_ERR_CODE_SUN_POSITION_NOT_INITIALIZED;
    }

    if (buffer == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    uint8_t index = manager.readIndex;
    manager.readIndex = (index + 1) % ADCS_POSITION_DATA_MANAGER_SIZE;  // Increment readIndex
    *buffer = manager.data[index];
    return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief Writes the data to the manager at writeIndex and shifts the writeIndex over
 * 
 * @attention manager must be a valid pointer; all julian dates in the manager and data are unique [not tested]
 *
 * @param data The data to be written to the manager
 * 
*/
static obc_error_code_t managerWriteData(position_data_t data) {
    if(!initialized) {
        return OBC_ERR_CODE_SUN_POSITION_NOT_INITIALIZED;
    }

    // Check if the julian date is unique
    julian_date_t maxJulianDate;
    obc_error_code_t errCode = managerGetMaxJulianDate(&maxJulianDate);
    if (errCode != OBC_ERR_CODE_SUCCESS) {
        return errCode;
    }
    if (data.julianDate <= maxJulianDate) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    // Write the data
    uint8_t index = manager.writeIndex;
    manager.data[index] = data;
    manager.writeIndex = (index + 1) % ADCS_POSITION_DATA_MANAGER_SIZE;
    return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief Searchs linearly for the julianDate in the manager and returns the index of the julian date in the manager that 
 * is greater or equal to the julianDate argument
 * @attention julian_data is greater than 0, module must be initialized
*/
static int managerSearchLinear(julian_date_t julianDate) {
    if (!initialized) {
        return OBC_ERR_CODE_SUN_POSITION_NOT_INITIALIZED;
    }

    if (julianDate <= ADCS_INVALID_JULIAN_DATE) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    uint8_t writeIndex = manager.writeIndex;
    uint8_t index = writeIndex;

    while (1) {
        if (julianDate <= (manager.data[index]).julianDate) {
            return index;
        } else if ((index + 1) % ADCS_POSITION_DATA_MANAGER_SIZE == writeIndex) {
            return index;
        }
        index = (index + 1) % ADCS_POSITION_DATA_MANAGER_SIZE;
    }
}

/**
 * @brief Searchs for the julianDate in the manager and returns the index of the julian date in the manager that 
 * is greater or equal to the julianDate
 * @attention manager must be a valid pointer and julianData is greater than 0
*/
static int managerSearch(julian_date_t julianDate) {
    if (!initialized) {
        return OBC_ERR_CODE_SUN_POSITION_NOT_INITIALIZED;
    }

    if (julianDate <= ADCS_INVALID_JULIAN_DATE) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    uint8_t low = manager.writeIndex;
    uint8_t high = (low - 1 + ADCS_POSITION_DATA_MANAGER_SIZE) % ADCS_POSITION_DATA_MANAGER_SIZE;
    uint8_t mid;  // Will be set later

    // Edge cases:
    if (julianDate >= managerGetJulianDateByIndex(high)) {
        return high;
    } else if (julianDate <= managerGetJulianDateByIndex(low)) {
        return low;
    }

    while (1) {
        if (low > high) {
            mid = ((low + high + ADCS_POSITION_DATA_MANAGER_SIZE) / 2) % ADCS_POSITION_DATA_MANAGER_SIZE;
        } else if (low < high) {
            mid = (low + high) / 2;
        } else {
            // low == high
            if (julianDate <= managerGetJulianDateByIndex(low)) {
                return low;
            } else {
                return (low + 1) % ADCS_POSITION_DATA_MANAGER_SIZE;
            }
        }
        int foundJulianDate = managerGetJulianDateByIndex(mid);

        if (foundJulianDate == julianDate) {
            return mid;
        } else if (foundJulianDate < julianDate) {
            low = (mid + 1) % ADCS_POSITION_DATA_MANAGER_SIZE;
        } else {
            high = (mid - 1 + ADCS_POSITION_DATA_MANAGER_SIZE) % ADCS_POSITION_DATA_MANAGER_SIZE;
        }

    }
}

/**
 * @brief Calculates the value of the point based on the target JD using the point1 as the lower value in the linear 
 * interpolation
 * 
 * @attention requires jd1 is not equal to jd2
 * 
 * @param targetJulianDate The data for the julian date we want
 * @param point1 The 1st point we are using to linearly interpolate
 * @param point2 The 2nd point we are using to linearly interpolate
 * @param jd1 The 1st julian date we are using to linearly interpolate
 * @param jd2 The 1st julian date we are using to linearly interpolate
 * 
 * @return Returns the value of the point at the targetJulanDate liner interpolated
*/
static obc_error_code_t linearlyInterpolate(julian_date_t targetJulianDate, 
                                      position_t point1, 
                                      position_t point2, 
                                      julian_date_t jd1, 
                                      julian_date_t jd2,
                                      position_t *buffer) {
    if (jd1 == jd2 || buffer == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    *buffer = point1 + ((point1 - point2) / (jd1 - jd2)) * (targetJulianDate - jd1);
    return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief Gets the adjusted value at the given julian date
 * 
 * @attention Requires buffer is a valid pointer; julianDate is greater or equal to the min julian date stored in manager;
 * module must be initialized
 * 
 * @param manager The manager from which to get the corresponding position data at the julianDate parameter
 * @param julianDate The julian date for which we want to get the data from the manager
 * 
 * @return The position data taken from the manager at given julian date
*/
static obc_error_code_t managerGetPositionData(julian_date_t julianDate, position_data_t *buffer) {
    if (!initialized) {
        return OBC_ERR_CODE_SUN_POSITION_NOT_INITIALIZED;
    }

    if (buffer == NULL || julianDate <= ADCS_INVALID_JULIAN_DATE) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    // Check that the julian date is greater than or equal to than the min julian date
    julian_date_t minJulianDate;
    obc_error_code_t error_code = managerGetMinJulianDate(&minJulianDate);
    if(error_code != OBC_ERR_CODE_SUCCESS) {
        return error_code;
    }
    if (julianDate < minJulianDate) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    int index = managerSearch(julianDate);
    position_data_t dataHigher;
    error_code = managerGetPositionDataByIndex(index, &dataHigher);

    if (error_code != OBC_ERR_CODE_SUCCESS) {
        return error_code;
    }

    // Main logic starts here

    if (dataHigher.julianDate == julianDate) {
        memcpy(buffer, &dataHigher, sizeof(position_data_t));
        return OBC_ERR_CODE_SUCCESS;
    } 

    position_data_t dataLower;
    error_code = managerGetPositionDataByIndex(index - 1 + ADCS_POSITION_DATA_MANAGER_SIZE, &dataLower);
    if (error_code != OBC_ERR_CODE_SUCCESS) {
        return error_code;
    }

    // Shouldn't happen
    if (dataLower.julianDate == dataHigher.julianDate) {
        return OBC_ERR_CODE_INVALID_STATE;
    }

    position_data_t newData = {julianDate, 0, 0, 0};
    // Buffer is valid so no need to check it
    (void) linearlyInterpolate(julianDate, dataLower.x, dataHigher.x, dataLower.julianDate, dataHigher.julianDate, &(newData.x));
    (void) linearlyInterpolate(julianDate, dataLower.y, dataHigher.y, dataLower.julianDate, dataHigher.julianDate, &(newData.y));
    (void) linearlyInterpolate(julianDate, dataLower.z, dataHigher.z, dataLower.julianDate, dataHigher.julianDate, &(newData.z));
    memcpy(buffer, &newData, sizeof(position_data_t));
    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t sunPositionGet(julian_date_t jd, position_data_t *buffer) {
    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t sunPositionNext(position_data_t *buffer) {
    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t sunPositionShiftTo(julian_date_t jd) {
    return OBC_ERR_CODE_SUCCESS;
}
