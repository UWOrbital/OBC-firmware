#include "sun_mgr.h"

#include "obc_errors.h"

#include <gtest/gtest.h>

TEST(TestObcSunMgr, writeUntilEnd) {
    position_data_manager_t manager = {0};
    ASSERT_EQ(sunManagerInit(&manager), OBC_ERR_CODE_SUCCESS);
    uint8_t count = 0;

    while(1) {
        position_data_t data = {1, 0, 0, 0};
        if(sunManagerWriteData(&manager, data) != OBC_ERR_CODE_SUCCESS) break;
        count++;
    }

    ASSERT_EQ(count, ADCS_POSITION_DATA_MANAGER_SIZE);
}

TEST(TestObcSunMgr, readUntilEnd) {
    position_data_manager_t manager = {0};
    ASSERT_EQ(sunManagerInit(&manager), OBC_ERR_CODE_SUCCESS);

    // Writes until the end of the manager
    uint8_t countWrite = 0;
    while(1) {
        position_t d = (position_t) countWrite;
        position_data_t data = {(julian_date_t) (countWrite + 1), d, d, d};
        if(sunManagerWriteData(&manager, data) != OBC_ERR_CODE_SUCCESS) break;
        countWrite++;
    }
    ASSERT_EQ(countWrite, ADCS_POSITION_DATA_MANAGER_SIZE);

    // Reads until the end of the manager
    uint8_t countRead = 0;
    while(1) {
        position_data_t dataRead;
        if(sunManagerReadData(&manager, &dataRead) != OBC_ERR_CODE_SUCCESS) break;
        position_t d2 = (position_t) countWrite;
        position_data_t dataExpect = {(julian_date_t) (countRead + 1), d2, d2, d2};
        countRead++;
        ASSERT_EQ(equalsDataPoint(dataRead, dataExpect), 1);
    }

    ASSERT_EQ(countRead, ADCS_POSITION_DATA_MANAGER_SIZE);
}
