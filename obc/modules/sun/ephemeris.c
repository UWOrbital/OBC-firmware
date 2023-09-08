#include "ephermeris.h"

#include <os_semphr.h>
#include <FreeRTOS.h>
#include <string.h>

#define ADCS_INVALID_JULIAN_DATE 0U

typedef uint8_t manager_size_t;

// uint8_t are used as the data manager will never exceed 256 data points as that is nearly 3 kb of data
/**
 * @struct	position_data_manager_t
 * @brief	Holds the position data and read/write indices. 
 * Read/write indices are the next indices to read from or write to
*/
typedef struct position_data_manager_t {
    manager_size_t readIndex;  // Will not exceed writeIndex
    manager_size_t writeIndex;
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

/**
 * @brief Initializes the manager
 * @attention Requires manager is a valid pointer
*/
static obc_error_code_t initSunPositionManager(position_data_manager_t *manager) {
    if (manager == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    manager->readIndex = 0;
    manager->writeIndex = 0;

    for (uint8_t i = 0; i < ADCS_POSITION_DATA_MANAGER_SIZE; i++) {
        position_data_t data = {ADCS_INVALID_JULIAN_DATE, 0, 0, 0};
        memcpy(&(manager->data[i]), &data, sizeof(position_data_t));
    }

    return OBC_ERR_CODE_SUCCESS;
}

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
        ASSERT(initSunPositionManager(&manager) == OBC_ERR_CODE_SUCCESS);
        initialized = 1;
    }
}   

/**
 * @brief Stores the data position at the given index % ADCS_POSITION_DATA_SIZE of the manager
 * @attention Requires buffer and manager must be valid pointers
*/
static obc_error_code_t managerGetPositionDataByIndex(position_data_manager_t *manager, manager_size_t index, position_data_t *buffer) {
    if (buffer == NULL || manager == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    *buffer = manager->data[index % ADCS_POSITION_DATA_MANAGER_SIZE];
    return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief Stores the julian date into the buffer at the given index in the manager
 * @attention Requires buffer and manager must be valid pointers
*/
static obc_error_code_t managerGetJulianDateByIndex(position_data_manager_t *manager, manager_size_t index, julian_date_t *buffer) {
    if (buffer == NULL || manager == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    position_data_t tmpData;
    obc_error_code_t errCode;
    RETURN_IF_ERROR_CODE(managerGetPositionDataByIndex(manager, index, &tmpData));

    *buffer = tmpData.julianDate;
    return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief Stores the minimum julian date into the buffer from the manager
 * @attention Requires buffer and manager must be valid pointers
*/
static obc_error_code_t managerGetMinJulianDate(position_data_manager_t *manager, julian_date_t *buffer) {
    if (buffer == NULL || manager == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    *buffer = manager->data[manager->writeIndex].julianDate;
    return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief Stores the maximum julian date into the buffer from the manager
 * @attention Requires buffer and manager must be valid pointers
*/
static obc_error_code_t managerGetMaxJulianDate(position_data_manager_t *manager, julian_date_t *buffer) {
    if (buffer == NULL || manager == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    manager_size_t index = (manager->writeIndex - 1 + ADCS_POSITION_DATA_MANAGER_SIZE) % ADCS_POSITION_DATA_MANAGER_SIZE;
    *buffer = manager->data[index].julianDate;
    return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief Reads the data from the manager into the buffer at readIndex and shifts the readIndex over
 * @attention buffer and manager must be valid pointers
*/
static obc_error_code_t managerReadData(position_data_manager_t *manager, position_data_t *buffer) {
    if (buffer == NULL || manager == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    manager_size_t index = manager->readIndex;
    manager->readIndex = (index + 1) % ADCS_POSITION_DATA_MANAGER_SIZE;  // Increment readIndex
    *buffer = manager->data[index];
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
static obc_error_code_t managerWriteData(position_data_manager_t *manager, position_data_t data) {
    if (manager == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }
    obc_error_code_t errCode;

    // Check if the julian date is unique
    julian_date_t maxJulianDate;
    RETURN_IF_ERROR_CODE(managerGetMaxJulianDate(manager, &maxJulianDate)) ;
    if (data.julianDate <= maxJulianDate) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    // Write the data
    manager_size_t index = manager->writeIndex;
    manager->data[index] = data;
    manager->writeIndex = (index + 1) % ADCS_POSITION_DATA_MANAGER_SIZE; // Increment writeIndex
    return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief Searchs linearly for the julianDate in the manager and returns the index of the julian date in the manager that 
 * is greater or equal to the julianDate argument
 * @attention julian_data is greater than 0 and manager and buffer are valid pointers
*/
static obc_error_code_t managerSearchLinear(const position_data_manager_t *manager, julian_date_t julianDate, manager_size_t *buffer) {
    if (julianDate <= ADCS_INVALID_JULIAN_DATE || manager == NULL || buffer == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    manager_size_t writeIndex = manager->writeIndex;
    manager_size_t index = writeIndex;

    while (1) {
        // Index of first JD greater than or equal to the inputted julianDate or end of the manager
        if (julianDate <= (manager->data[index]).julianDate || (index + 1) % ADCS_POSITION_DATA_MANAGER_SIZE == writeIndex) {
            *buffer = index;
            return OBC_ERR_CODE_SUCCESS;
        }
        index = (index + 1) % ADCS_POSITION_DATA_MANAGER_SIZE;
    }
}

/**
 * @brief Searchs for the julianDate in the manager and returns the index of the julian date in the manager that 
 * is greater or equal to the julianDate
 * @attention manager must be a valid pointer and julianData is greater than 0
*/
static obc_error_code_t managerSearch(const position_data_manager_t *manager, julian_date_t julianDate, manager_size_t *buffer) {
    if (julianDate <= ADCS_INVALID_JULIAN_DATE || manager == NULL || buffer == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    manager_size_t low = manager->writeIndex;
    manager_size_t high = (low - 1 + ADCS_POSITION_DATA_MANAGER_SIZE) % ADCS_POSITION_DATA_MANAGER_SIZE;
    manager_size_t mid;  // Will be set later

    // Get the data for the high and low indices (edge cases)
    manager_size_t highJD, lowJD;
    obc_error_code_t errCode;
    RETURN_IF_ERROR_CODE(managerGetJulianDateByIndex(manager, high, &highJD));
    RETURN_IF_ERROR_CODE(managerGetJulianDateByIndex(manager, low, &lowJD));

    // Handle edge cases
    if (julianDate >= highJD) {
        *buffer = high;
        return OBC_ERR_CODE_SUCCESS;
    } else if (julianDate <= lowJD) {
        *buffer = low;
        return OBC_ERR_CODE_SUCCESS;
    }

    while (1) {
        if (low > high) {
            mid = ((low + high + ADCS_POSITION_DATA_MANAGER_SIZE) / 2) % ADCS_POSITION_DATA_MANAGER_SIZE;
        } else if (low < high) {
            mid = (low + high) / 2;
        } else {
            // low == high
            julian_date_t julianDate;
            RETURN_IF_ERROR_CODE(managerGetJulianDateByIndex(manager, low, &julianDate));
            if (julianDate <= julianDate) {
                *buffer = low;
                return OBC_ERR_CODE_SUCCESS;
            } else {
                *buffer = (low + 1) % ADCS_POSITION_DATA_MANAGER_SIZE;
                return OBC_ERR_CODE_SUCCESS;
            }
        }

        julian_date_t foundJulianDate;
        RETURN_IF_ERROR_CODE(managerGetJulianDateByIndex(manager, mid, &foundJulianDate));

        if (foundJulianDate == julianDate) {
            *buffer = mid;
            return OBC_ERR_CODE_SUCCESS;
        } else if (foundJulianDate < julianDate) {
            low = (mid + 1) % ADCS_POSITION_DATA_MANAGER_SIZE;
        } else {
            high = (mid - 1 + ADCS_POSITION_DATA_MANAGER_SIZE) % ADCS_POSITION_DATA_MANAGER_SIZE;
        }

    }
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
static obc_error_code_t managerGetPositionData(position_data_manager_t *manager, julian_date_t julianDate, position_data_t *buffer) {
    if (buffer == NULL || julianDate <= ADCS_INVALID_JULIAN_DATE || manager == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }
    obc_error_code_t errCode;

    // Check that the julian date is greater than or equal to than the min julian date
    julian_date_t minJulianDate;
    RETURN_IF_ERROR_CODE(managerGetMinJulianDate(manager, &minJulianDate));
    if (julianDate < minJulianDate) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    // Search for the index of the closest julian date
    manager_size_t index;
    RETURN_IF_ERROR_CODE(managerSearch(manager, julianDate, &index));

    position_data_t dataHigher;
    RETURN_IF_ERROR_CODE(managerGetPositionDataByIndex(manager, index, &dataHigher));

    // Main logic starts here

    if (dataHigher.julianDate == julianDate) {
        memcpy(buffer, &dataHigher, sizeof(position_data_t));
        return OBC_ERR_CODE_SUCCESS;
    } 

    position_data_t dataLower;
    RETURN_IF_ERROR_CODE(managerGetPositionDataByIndex(manager, index - 1 + ADCS_POSITION_DATA_MANAGER_SIZE, &dataLower));

    // Linearly interpolate the 2 closest data points
    position_data_t newData = {julianDate, 0, 0, 0};
    RETURN_IF_ERROR_CODE(linearlyInterpolate(julianDate, dataLower.x, dataHigher.x, dataLower.julianDate, dataHigher.julianDate, &(newData.x)));
    RETURN_IF_ERROR_CODE(linearlyInterpolate(julianDate, dataLower.y, dataHigher.y, dataLower.julianDate, dataHigher.julianDate, &(newData.y)));
    RETURN_IF_ERROR_CODE(linearlyInterpolate(julianDate, dataLower.z, dataHigher.z, dataLower.julianDate, dataHigher.julianDate, &(newData.z)));
    memcpy(buffer, &newData, sizeof(position_data_t));

    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t sunPositionGet(julian_date_t jd, position_data_t *buffer) {
    return OBC_ERR_CODE_SUN_POSITION_NOT_IMPLEMENTED;
}

obc_error_code_t sunPositionNext(position_data_t *buffer) {
    return OBC_ERR_CODE_SUN_POSITION_NOT_IMPLEMENTED;
}

obc_error_code_t sunPositionShiftTo(julian_date_t jd) {
    return OBC_ERR_CODE_SUN_POSITION_NOT_IMPLEMENTED;
}
