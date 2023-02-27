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
static void printPositionData(const position_data *data) {
    assert(data);
    printf("Julian Date: %f, X: %f, Y: %f, Z:%f\n", data->julian_date, data->x, data->y, data->z);
}

// For testing purposes only
static void printPositionDataManager(const position_data_manager *manager){
    assert(manager);
    for(int i=0; i<ADCS_POSITION_DATA_SIZE; i++) {
        printPositionData(&(manager->data[i]));
    }
    printf("Read Index: %d\n", manager->read_index);
    printf("Write Index: %d\n", manager->write_index);
}

/**
 * @brief Reads the data from the manager at read_index and shifts the read_index over
*/
static position_data readData(position_data_manager *manager) {
    assert(manager);
    int index = manager->read_index;
    manager->read_index = (index + 1) % ADCS_POSITION_DATA_SIZE;
    return manager->data[index];
}

/**
 * @brief Writes the data to the manager at write_index and shifts the write_index over
*/
static void writeData(position_data_manager *manager, position_data data) {
    assert(manager);
    int index = manager->write_index;
    manager->data[index] = data;
    manager->write_index = (index + 1) % ADCS_POSITION_DATA_SIZE;
}

static position_data initPositionData(void) {
    position_data data = {0, 0, 0, 0};
    return data;
}

/**
 * @brief Creates a position data manager struct of size ADCS_POSITION_DATA_SIZE. Could allow the function to
 * be flexible by taking in a length but then would need to use malloc and free and track the size in the struct.
*/
static position_data_manager initPositionDataManager(void) {
    position_data_manager manager;

    for(int i=0; i<ADCS_POSITION_DATA_SIZE; i++) {
        manager.data[i] = initPositionData();
    }

    manager.read_index = 0;
    manager.write_index = 0;
    return manager;
}

int main(void) {
    position_data_manager manager = initPositionDataManager();
    printPositionDataManager(&manager);

    // Writes a bunch of bogus data to the manager
    for(int i=1; i<12; i++) {
        position_data data = {i, i, i, i};
        writeData(&manager, data);
    }
    printPositionDataManager(&manager);

    // Reads 15 data points from the manager
    for (int i=0; i<15; i++) {
        position_data d = readData(&manager);
        printPositionData(&d);
    }
    printf("\n");

    printPositionDataManager(&manager);
    printf("%d\n", sizeof(manager));
    return 0;
}