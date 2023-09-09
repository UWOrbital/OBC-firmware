#include "sun_mgr.h"

obc_error_code_t sunManagerInit(position_data_manager_t *manager) {
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

/**
 * @brief Stores the data position at the given index % ADCS_POSITION_DATA_SIZE of the manager
 * @attention Requires buffer and manager must be valid pointers
*/
static obc_error_code_t sunManagerGetPositionDataByIndex(const position_data_manager_t *manager, manager_size_t index, position_data_t *buffer) {
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
static obc_error_code_t sunManagerGetJulianDateByIndex(const position_data_manager_t *manager, manager_size_t index, julian_date_t *buffer) {
    if (buffer == NULL || manager == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    position_data_t tmpData;
    obc_error_code_t errCode;
    RETURN_IF_ERROR_CODE(sunManagerGetPositionDataByIndex(manager, index, &tmpData));

    *buffer = tmpData.julianDate;
    return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief Stores the minimum julian date into the buffer from the manager
 * @attention Requires buffer and manager must be valid pointers
*/
static obc_error_code_t sunManagerGetMinJulianDate(const position_data_manager_t *manager, julian_date_t *buffer) {
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
static obc_error_code_t sunManagerGetMaxJulianDate(const position_data_manager_t *manager, julian_date_t *buffer) {
    if (buffer == NULL || manager == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    manager_size_t index = (manager->writeIndex - 1 + ADCS_POSITION_DATA_MANAGER_SIZE) % ADCS_POSITION_DATA_MANAGER_SIZE;
    *buffer = manager->data[index].julianDate;
    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t sunManagerReadData(position_data_manager_t *manager, position_data_t *buffer) {
    if (buffer == NULL || manager == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    // Read index is at the smallest julian date in manager which is where the data is to be written
    if (manager->readIndex == manager->writeIndex) {
        return OBC_ERR_CODE_SUN_POSITION_MGR_READ_FAIL;
    }

    manager_size_t index = manager->readIndex;
    manager->readIndex = (index + 1) % ADCS_POSITION_DATA_MANAGER_SIZE;  // Increment readIndex
    *buffer = manager->data[index];
    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t sunManagerWriteData(position_data_manager_t *manager, position_data_t data) {
    if (manager == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    // Next write index is the same as the read index so we don't want to override the data at the read index 
    if ((manager->writeIndex + 1) % ADCS_POSITION_DATA_MANAGER_SIZE == manager->readIndex) {
        return OBC_ERR_CODE_SUN_POSITION_MGR_WRITE_FAIL;
    }

    obc_error_code_t errCode;

    // Check if the julian date is unique, all data must be sorted by the JD in ascending order
    julian_date_t maxJulianDate;
    RETURN_IF_ERROR_CODE(sunManagerGetMaxJulianDate(manager, &maxJulianDate));
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
static obc_error_code_t sunManagerSearchLinear(const position_data_manager_t *manager, julian_date_t julianDate, manager_size_t *buffer) {
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
static obc_error_code_t sunManagerSearch(const position_data_manager_t *manager, julian_date_t julianDate, manager_size_t *buffer) {
    if (julianDate <= ADCS_INVALID_JULIAN_DATE || manager == NULL || buffer == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    manager_size_t low = manager->writeIndex;
    manager_size_t high = (low - 1 + ADCS_POSITION_DATA_MANAGER_SIZE) % ADCS_POSITION_DATA_MANAGER_SIZE;
    manager_size_t mid;  // Will be set later

    // Get the data for the high and low indices (edge cases)
    julian_date_t highJD, lowJD;
    obc_error_code_t errCode;
    RETURN_IF_ERROR_CODE(sunManagerGetJulianDateByIndex(manager, high, &highJD));
    RETURN_IF_ERROR_CODE(sunManagerGetJulianDateByIndex(manager, low, &lowJD));

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
            RETURN_IF_ERROR_CODE(sunManagerGetJulianDateByIndex(manager, low, &julianDate));
            if (julianDate <= julianDate) {
                *buffer = low;
                return OBC_ERR_CODE_SUCCESS;
            } else {
                *buffer = (low + 1) % ADCS_POSITION_DATA_MANAGER_SIZE;
                return OBC_ERR_CODE_SUCCESS;
            }
        }

        julian_date_t foundJulianDate;
        RETURN_IF_ERROR_CODE(sunManagerGetJulianDateByIndex(manager, mid, &foundJulianDate));

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

obc_error_code_t sunManagerGetPositionData(const position_data_manager_t *manager, julian_date_t julianDate, position_data_t *buffer) {
    if (buffer == NULL || julianDate <= ADCS_INVALID_JULIAN_DATE || manager == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }
    obc_error_code_t errCode;

    // Check that the julian date is greater than or equal to than the min julian date
    julian_date_t minJulianDate;
    RETURN_IF_ERROR_CODE(sunManagerGetMinJulianDate(manager, &minJulianDate));
    if (julianDate < minJulianDate) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    // Search for the index of the closest julian date
    manager_size_t index;
    RETURN_IF_ERROR_CODE(sunManagerSearch(manager, julianDate, &index));

    position_data_t dataHigher;
    RETURN_IF_ERROR_CODE(sunManagerGetPositionDataByIndex(manager, index, &dataHigher));

    // Main logic starts here

    if (dataHigher.julianDate == julianDate) {
        memcpy(buffer, &dataHigher, sizeof(position_data_t));
        return OBC_ERR_CODE_SUCCESS;
    } 

    position_data_t dataLower;
    RETURN_IF_ERROR_CODE(sunManagerGetPositionDataByIndex(manager, index - 1 + ADCS_POSITION_DATA_MANAGER_SIZE, &dataLower));

    // Linearly interpolate the 2 closest data points
    position_data_t newData = {julianDate, 0, 0, 0};
    RETURN_IF_ERROR_CODE(linearlyInterpolate(julianDate, dataLower.x, dataHigher.x, dataLower.julianDate, dataHigher.julianDate, &(newData.x)));
    RETURN_IF_ERROR_CODE(linearlyInterpolate(julianDate, dataLower.y, dataHigher.y, dataLower.julianDate, dataHigher.julianDate, &(newData.y)));
    RETURN_IF_ERROR_CODE(linearlyInterpolate(julianDate, dataLower.z, dataHigher.z, dataLower.julianDate, dataHigher.julianDate, &(newData.z)));
    memcpy(buffer, &newData, sizeof(position_data_t));

    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t sunManagerCheckJD(const position_data_manager_t *manager, julian_date_t jd, uint8_t *buffer) {
    if (manager == NULL || jd <= ADCS_INVALID_JULIAN_DATE || buffer == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    obc_error_code_t errCode;

    julian_date_t minJulianDate;
    RETURN_IF_ERROR_CODE(sunManagerGetMinJulianDate(manager, &minJulianDate));
    julian_date_t maxJulianDate;
    RETURN_IF_ERROR_CODE(sunManagerGetMaxJulianDate(manager, &maxJulianDate));

    *buffer = (jd >= minJulianDate && jd <= maxJulianDate);
    return OBC_ERR_CODE_SUCCESS;
}
