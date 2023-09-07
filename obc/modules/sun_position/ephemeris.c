#include "ephermeris.h"

/**
 * @brief Returns the position at the given index % ADCS_POSITION_DATA_SIZE of the manager
 * @attention Requires index is non-zero and manager is a valid pointer
*/
static position_data_t getPositionDataByIndex(const position_data_manager_t *manager, int index) {
    assert(manager);
    assert(index >= 0);
    return manager->data[index % ADCS_POSITION_DATA_MANAGER_SIZE];
}

/**
 * @brief get the julian data at the given index in the manager
 * @attention requires manager is a valid pointer and index >= 0
*/
static julian_date_t getJulianDateByIndex(const position_data_manager_t *manager, int index) {
    assert(manager);
    assert(index >= 0);
    return getPositionDataByIndex(manager, index).julianDate;
}

julian_date_t getMinJulianDate(const position_data_manager_t *manager) {
    assert(manager);
    return manager->data[manager->writeIndex].julianDate;
}

julian_date_t getMaxJulianDate(const position_data_manager_t *manager) {
    assert(manager);
    return manager->data[(manager->writeIndex + ADCS_POSITION_DATA_MANAGER_SIZE - 1) % ADCS_POSITION_DATA_MANAGER_SIZE].julianDate;
}

/**
 * @brief Reads the data from the manager at readIndex and shifts the readIndex over
 * @attention manager must be a valid pointer to a position_data_manager_t
 * @param manager The manager to be read from
 * @return The position data at the current readIndex before shifting 
*/
static position_data_t readData(position_data_manager_t *manager) {
    assert(manager);
    int index = manager->readIndex;
    manager->readIndex = (index + 1) % ADCS_POSITION_DATA_MANAGER_SIZE;
    return manager->data[index];
}

/**
 * @brief Writes the data to the manager at writeIndex and shifts the writeIndex over
 * 
 * @attention manager must be a valid pointer; all julian dates in the manager and data are unique [not tested]
 * 
 * @param manager The manager to be written to
 * @param data The data to be written
 * 
*/
static void writeData(position_data_manager_t *manager, position_data_t data) {
    assert(manager);
    assert((getMaxJulianDate(manager) < data.julianDate));

    int index = manager->writeIndex;
    manager->data[index] = data;
    manager->writeIndex = (index + 1) % ADCS_POSITION_DATA_MANAGER_SIZE;
}

/**
 * @brief Searchs linearly for the julianDate in the manager and returns the index of the julian date in the manager that 
 * is greater or equal to the julianDate argument
 * @attention manager must be a valid pointer and julian_data is greater than 0
*/
static int searchManagerLinear(const position_data_manager_t *manager, float julianDate) {
    assert(manager);
    assert(julianDate > ADCS_INVALID_JULIAN_DATE);

    size_t writeIndex = manager->writeIndex;
    size_t index = writeIndex;

    while (1) {
        if (julianDate <= (manager->data[index]).julianDate) {
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
static int searchManager(const position_data_manager_t *manager, float julianDate) {
    assert(manager);
    assert(julianDate > ADCS_INVALID_JULIAN_DATE);
    int low = manager->writeIndex;
    int high = (low - 1 + ADCS_POSITION_DATA_MANAGER_SIZE) % ADCS_POSITION_DATA_MANAGER_SIZE;
    int mid = -1;

    // Edge cases:
    if (julianDate >= getJulianDateByIndex(manager, high)) {
        return high;
    } else if (julianDate <= getJulianDateByIndex(manager, low)) {
        return low;
    }

    while (1) {
        if (low > high) {
            mid = ((low + high + ADCS_POSITION_DATA_MANAGER_SIZE) / 2) % ADCS_POSITION_DATA_MANAGER_SIZE;
        } else if (low < high) {
            mid = (low + high) / 2;
        } else {
            // low == high
            if (julianDate <= getJulianDateByIndex(manager, low)) {
                return low;
            } else {
                return (low + 1) % ADCS_POSITION_DATA_MANAGER_SIZE;
            }
        }
        int foundJulianDate = getJulianDateByIndex(manager, mid);

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
static float linearlyInterpolate(float targetJulianDate, float point1, float point2, float jd1, float jd2) {
    assert(jd1 != jd2);
    return point1 + ((point1 - point2) / (jd1 - jd2)) * (targetJulianDate - jd1);
}

position_data_t getPositionData(const position_data_manager_t *manager, float julianDate) {
    assert(manager);
    assert(julianDate >= getMinJulianDate(manager));

    int index = searchManager(manager, julianDate);
    position_data_t dataHigher = getPositionDataByIndex(manager, index);

    if (dataHigher.julianDate == julianDate) {
        return dataHigher;
    } else {
        position_data_t dataLower = getPositionDataByIndex(manager, index - 1 + ADCS_POSITION_DATA_MANAGER_SIZE);
        position_data_t newData = {julianDate, 0, 0, 0};
        newData.x = linearlyInterpolate(julianDate, dataLower.x, dataHigher.x, dataLower.julianDate, dataHigher.julianDate);
        newData.y = linearlyInterpolate(julianDate, dataLower.y, dataHigher.y, dataLower.julianDate, dataHigher.julianDate);
        newData.z = linearlyInterpolate(julianDate, dataLower.z, dataHigher.z, dataLower.julianDate, dataHigher.julianDate);
        return newData;
    } 
}

position_data_manager_t initPositionDataManager(void) {
    position_data_manager_t manager;

    for(unsigned int i=0; i<ADCS_POSITION_DATA_MANAGER_SIZE; i++) {
        position_data_t data = {ADCS_INVALID_JULIAN_DATE, 0, 0, 0};
        manager.data[i] = data;
    }

    manager.readIndex = 0;
    manager.writeIndex = 0;
    return manager;
}