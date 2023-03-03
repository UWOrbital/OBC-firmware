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
    for(int i=0; i<ADCS_POSITION_DATA_SIZE; i++) {
        printPositionData(&(manager->data[i]));
    }
    printf("Read Index: %zu\n", manager->readIndex);
    printf("Write Index: %zu\n", manager->writeIndex);
}

/**
 * @brief Returns true if data1 and data2 are identifical. 
 * @attention USED ONLY FOR TESTING PURPOSES.
*/
static int positionDataEquals(const position_data_t data1, const position_data_t data2) {
    return data1.julianDate == data2.julianDate && data1.x == data2.x && data1.y == data2.y && data1.z == data2.z;
}

/**
 * @brief Returns the position at the given index % ADCS_POSITION_DATA_SIZE of the manager
 * @attention Requires index is non-zero and manager is a valid pointer
*/
static position_data_t getPositionData(const position_data_manager_t *manager, int index) {
    assert(manager);
    assert(index >= 0);
    return manager->data[index % ADCS_POSITION_DATA_SIZE];
}

/**
 * @brief get the julian data at the given index in the manager
 * @attention requires manager is a valid pointer and index >= 0
*/
static float getJulianDate(const position_data_manager_t *manager, int index) {
    assert(manager);
    assert(index >= 0);
    return getPositionData(manager, index).julianDate;
}

/**
 * @brief Reads the data from the manager at read_index and shifts the read_index over
*/
static position_data_t readData(position_data_manager_t *manager) {
    assert(manager);
    int index = manager->readIndex;
    manager->readIndex = (index + 1) % ADCS_POSITION_DATA_SIZE;
    return manager->data[index];
}

/**
 * @brief Writes the data to the manager at write_index and shifts the write_index over
 * @attention manager must be a valid pointer; all julian dates in the manager and data are unique [not tested]
*/
static void writeData(position_data_manager_t *manager, position_data_t data) {
    assert(manager);
    int index = manager->writeIndex;
    assert((getJulianDate(manager, index - 1 + ADCS_POSITION_DATA_SIZE) < data.julianDate));

    manager->data[index] = data;
    manager->writeIndex = (index + 1) % ADCS_POSITION_DATA_SIZE;
}

/**
 * @brief Gets the minimum julian date 
 * @attention Requires manager is a valid pointer
*/
static float getMinJulianDate(const position_data_manager_t *manager) {
    assert(manager);
    return manager->data[manager->writeIndex].julianDate;
}

/**
 * @brief Gets the minimum julian date 
 * @attention Requires manager is a valid pointer
*/
static float getMaxJulianDate(const position_data_manager_t *manager) {
    assert(manager);
    return manager->data[(manager->writeIndex + ADCS_POSITION_DATA_SIZE - 1) % ADCS_POSITION_DATA_SIZE].julianDate;
}


/**
 * @brief Searchs linearly for the julian_date in the manager and returns the index of the julian date in the manager that 
 * is greater or equal to the julian_date
 * @attention manager must be a valid pointer and julian_data is greater than 0
*/
static int searchManagerLinear(const position_data_manager_t *manager, float julianDate) {
    assert(manager);
    assert(julianDate > 0);

    int writeIndex = manager->writeIndex;
    int index = writeIndex;

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
 * @attention manager must be a valid pointer and julian_data is greater than 0
*/
static int searchManager(const position_data_manager_t *manager, float julianDate) {
    assert(manager);
    assert(julianDate > 0);
    int low = manager->writeIndex;
    int high = (low - 1 + ADCS_POSITION_DATA_SIZE) % ADCS_POSITION_DATA_SIZE;
    int mid = -1;

    // Edge cases:
    if (julianDate >= getJulianDate(manager, high)) {
        return high;
    } else if (julianDate <= getJulianDate(manager, low)) {
        return low;
    }

    while (1) {
        if (low > high) {
            mid = ((low + high + ADCS_POSITION_DATA_SIZE) / 2) % ADCS_POSITION_DATA_SIZE;
        } else if (low < high) {
            mid = (low + high) / 2;
        } else {
            // low == high
            if (julianDate <= getJulianDate(manager, low)) {
                return low;
            } else {
                return (low + 1) % ADCS_POSITION_DATA_SIZE;
            }
        }
        int foundJulianDate = getJulianDate(manager, mid);

        if (foundJulianDate == julianDate) {
            return mid;
        } else if (foundJulianDate < julianDate) {
            low = (mid + 1) % ADCS_POSITION_DATA_SIZE;
        } else {
            high = (mid - 1 + ADCS_POSITION_DATA_SIZE) % ADCS_POSITION_DATA_SIZE;
        }

    }
    // Never supposed to get here
    return -1;
}

/**
 * @brief Calculates the value of the point based on the target JD using the point1 as the lower value in the linear 
 * interpolation
 * @attention requires jd1 is not equal to jd2
*/
static float calculateValue(float targetJulanDate, float point1, float point2, float jd1, float jd2) {
    assert(jd1 != jd2);
    return point1 + ((point1 - point2) / (jd1 - jd2)) * (targetJulanDate - jd1);
}

/**
 * @brief Gets the adjusted value at the given julian date
 * @attention Requires manager is a valid pointer and julian date greater or equal than the min julian date stored in manager
*/
static position_data_t getPositionDataAdjusted(const position_data_manager_t *manager, float julianDate) {
    assert(manager);
    assert(julianDate >= getMinJulianDate(manager));

    int index = searchManager(manager, julianDate);
    position_data_t dataHigher = getPositionData(manager, index);

    if (dataHigher.julianDate == julianDate) {
        return dataHigher;
    } else {
        position_data_t dataLower = getPositionData(manager, index - 1 + ADCS_POSITION_DATA_SIZE);
        position_data_t newData = {julianDate};
        newData.x = calculateValue(julianDate, dataLower.x, dataHigher.x, dataLower.julianDate, dataHigher.julianDate);
        newData.y = calculateValue(julianDate, dataLower.y, dataHigher.y, dataLower.julianDate, dataHigher.julianDate);
        newData.z = calculateValue(julianDate, dataLower.z, dataHigher.z, dataLower.julianDate, dataHigher.julianDate);
        return newData;
    } 
}

/**
 * @brief Initializes a position data point
*/
static position_data_t initPositionData(void) {
    position_data_t data = {0, 0, 0, 0};
    return data;
}

/**
 * @brief Creates a position data manager struct of size ADCS_POSITION_DATA_SIZE. Could allow the function to
 * be flexible by taking in a length but then would need to use malloc and free and track the size in the struct.
*/
static position_data_manager_t initPositionDataManager(void) {
    position_data_manager_t manager;

    for(int i=0; i<ADCS_POSITION_DATA_SIZE; i++) {
        manager.data[i] = initPositionData();
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
        float jd = getJulianDate(&manager, i);
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
    assert(getPositionData(&manager, 0).julianDate == 11);
    assert(getPositionData(&manager, 1).julianDate == 2);
    assert(getPositionData(&manager, 9).julianDate == 10);
    assert(getPositionData(&manager, 10).julianDate == 11);
    assert(getPositionData(&manager, 5).julianDate == 6);

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
    assert(positionDataEquals(data1, data2));
    data2.x = 0;
    assert(!positionDataEquals(data1, data2));

// tests interpolation and extrapolation
    position_data_t data3 = {2, 4, 6, -2};
    position_data_t data4 = {5, 10, 15, -5};
    position_data_t data5 = {6.9, 13.8, 20.7, -6.9};
    position_data_t data6 = {13, 26, 39, -13};
    assert(positionDataEquals(getPositionDataAdjusted(&manager, 2), data3));
    assert(positionDataEquals(getPositionDataAdjusted(&manager, 5), data4));
    assert(positionDataEquals(getPositionDataAdjusted(&manager, 6.9), data5));
    assert(positionDataEquals(getPositionDataAdjusted(&manager, 13), data6));

    printf("Assertions passed\n");
    return 0;
}