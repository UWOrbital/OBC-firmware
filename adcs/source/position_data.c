#include "position_data.h"
#include "obc_errors.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_queue.h>
#include <os_task.h>

// For testing purposes only
// #include <sys_common.h>
// #include <gio.h>
#include <stdio.h>

// Replace all asserts with ASSERT
#include <assert.h>

// For testing purposes only
static void printPositionData(const position_data_t *data) {
    assert(data);
    printf("Julian Date: %f, X: %f, Y: %f, Z:%f\n", data->julianDate, data->x, data->y, data->z);
}

// For testing purposes only
static void printPositionDataManager(const position_data_manager_t *manager){
    assert(manager);
    for(unsigned int i=0; i<ADCS_POSITION_DATA_SIZE; i++) {
        printPositionData(&(manager->data[i]));
    }
    printf("Read Index: %zu\n", manager->readIndex);
    printf("Write Index: %zu\n", manager->writeIndex);
}

int equalsPositionData(const position_data_t data1, const position_data_t data2) {
    return data1.julianDate == data2.julianDate && data1.x == data2.x && data1.y == data2.y && data1.z == data2.z;
}

/**
 * @brief Returns the position at the given index % ADCS_POSITION_DATA_SIZE of the manager
 * @attention Requires index is non-zero and manager is a valid pointer
*/
static position_data_t getPositionDataByIndex(const position_data_manager_t *manager, int index) {
    assert(manager);
    assert(index >= 0);
    return manager->data[index % ADCS_POSITION_DATA_SIZE];
}

/**
 * @brief get the julian data at the given index in the manager
 * @attention requires manager is a valid pointer and index >= 0
*/
static float getJulianDateByIndex(const position_data_manager_t *manager, int index) {
    assert(manager);
    assert(index >= 0);
    return getPositionDataByIndex(manager, index).julianDate;
}

float getMinJulianDate(const position_data_manager_t *manager) {
    assert(manager);
    return manager->data[manager->writeIndex].julianDate;
}

float getMaxJulianDate(const position_data_manager_t *manager) {
    assert(manager);
    return manager->data[(manager->writeIndex + ADCS_POSITION_DATA_SIZE - 1) % ADCS_POSITION_DATA_SIZE].julianDate;
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
    manager->readIndex = (index + 1) % ADCS_POSITION_DATA_SIZE;
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
    manager->writeIndex = (index + 1) % ADCS_POSITION_DATA_SIZE;
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
        } else if ((index + 1) % ADCS_POSITION_DATA_SIZE == writeIndex) {
            return index;
        }
        index = (index + 1) % ADCS_POSITION_DATA_SIZE;
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
    int high = (low - 1 + ADCS_POSITION_DATA_SIZE) % ADCS_POSITION_DATA_SIZE;
    int mid = -1;

    // Edge cases:
    if (julianDate >= getJulianDateByIndex(manager, high)) {
        return high;
    } else if (julianDate <= getJulianDateByIndex(manager, low)) {
        return low;
    }

    while (1) {
        if (low > high) {
            mid = ((low + high + ADCS_POSITION_DATA_SIZE) / 2) % ADCS_POSITION_DATA_SIZE;
        } else if (low < high) {
            mid = (low + high) / 2;
        } else {
            // low == high
            if (julianDate <= getJulianDateByIndex(manager, low)) {
                return low;
            } else {
                return (low + 1) % ADCS_POSITION_DATA_SIZE;
            }
        }
        int foundJulianDate = getJulianDateByIndex(manager, mid);

        if (foundJulianDate == julianDate) {
            return mid;
        } else if (foundJulianDate < julianDate) {
            low = (mid + 1) % ADCS_POSITION_DATA_SIZE;
        } else {
            high = (mid - 1 + ADCS_POSITION_DATA_SIZE) % ADCS_POSITION_DATA_SIZE;
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
static float calculateValue(float targetJulianDate, float point1, float point2, float jd1, float jd2) {
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
        position_data_t dataLower = getPositionDataByIndex(manager, index - 1 + ADCS_POSITION_DATA_SIZE);
        position_data_t newData = {julianDate, 0, 0, 0};
        newData.x = calculateValue(julianDate, dataLower.x, dataHigher.x, dataLower.julianDate, dataHigher.julianDate);
        newData.y = calculateValue(julianDate, dataLower.y, dataHigher.y, dataLower.julianDate, dataHigher.julianDate);
        newData.z = calculateValue(julianDate, dataLower.z, dataHigher.z, dataLower.julianDate, dataHigher.julianDate);
        return newData;
    } 
}

/**
 * @brief Initializes a position data point
 * 
 * @param julianDate The julian data of the position data
 * @param x The x coordinate of the position data
 * @param y The y coordinate of the position data
 * @param z The z coordinate of the position data
 * 
 * @return The position data point with the given data
*/
static position_data_t initPositionData(float julianDate, float x, float y, float z) {
    position_data_t data = {julianDate, x, y, z};
    return data;
}

position_data_manager_t initPositionDataManager(void) {
    position_data_manager_t manager;

    for(unsigned int i=0; i<ADCS_POSITION_DATA_SIZE; i++) {
        manager.data[i] = initPositionData(ADCS_INVALID_JULIAN_DATE, 0, 0, 0);
    }

    manager.readIndex = 0;
    manager.writeIndex = 0;
    return manager;
}

int main(void) {
    position_data_manager_t manager = initPositionDataManager();
    printPositionDataManager(&manager);

    // Writes a bunch of bogus data to the manager
    for(int i=1; i<12; i++) {
        position_data_t data = {i, i*2, i*3, -i};
        writeData(&manager, data);
    }
    printPositionDataManager(&manager);

    // Reads 15 data points from the manager
    for (int i=0; i<15; i++) {
        position_data_t d = readData(&manager);
        printPositionData(&d);
        float jd = getJulianDateByIndex(&manager, i);
        printf("%f\n", jd);
    }
    printf("\n");

    printPositionDataManager(&manager);

    // Tests linear search based on above manager
    assert(searchManagerLinear(&manager, 1.5) == 1);
    assert(searchManagerLinear(&manager, 2) == 1);
    assert(searchManagerLinear(&manager, 5) == 4);
    assert(searchManagerLinear(&manager, 6.8) == 6);
    assert(searchManagerLinear(&manager, 11) == 0);
    assert(searchManagerLinear(&manager, 12) == 0);

// Tests access based on above manager
    assert(getPositionDataByIndex(&manager, 0).julianDate == 11);
    assert(getPositionDataByIndex(&manager, 1).julianDate == 2);
    assert(getPositionDataByIndex(&manager, 9).julianDate == 10);
    assert(getPositionDataByIndex(&manager, 10).julianDate == 11);
    assert(getPositionDataByIndex(&manager, 5).julianDate == 6);

// tests for bs based on above manager
    assert(searchManager(&manager, 1.5) == 1);
    assert(searchManager(&manager, 2) == 1);
    assert(searchManager(&manager, 2.2) == 2);
    assert(searchManager(&manager, 5) == 4);
    assert(searchManager(&manager, 6.8) == 6);
    assert(searchManager(&manager, 11) == 0);
    assert(searchManager(&manager, 10.7) == 0);
    assert(searchManager(&manager, 12) == 0); 

// tests the min and max data
    assert(getMinJulianDate(&manager) == 2);
    assert(getMaxJulianDate(&manager) == 11);
    position_data_t data1 = {1.2, -3.5, 4, 8};
    position_data_t data2 = {1.2, -3.5, 4, 8};
    assert(equalsPositionData(data1, data2));
    data2.x = 0;
    assert(!equalsPositionData(data1, data2));

// tests interpolation and extrapolation
    position_data_t data3 = {2, 4, 6, -2};
    position_data_t data4 = {5, 10, 15, -5};
    position_data_t data5 = {6.9, 13.8, 20.7, -6.9};
    position_data_t data6 = {13, 26, 39, -13};
    assert(equalsPositionData(getPositionData(&manager, 2), data3));
    assert(equalsPositionData(getPositionData(&manager, 5), data4));
    assert(equalsPositionData(getPositionData(&manager, 6.9), data5));
    assert(equalsPositionData(getPositionData(&manager, 13), data6));

    printf("Assertions passed\n");
    return 0;
}