#include "sun_mgr.h"
#include <stdio.h>

#include "obc_errors.h"

#include <gtest/gtest.h>

TEST(TestObcSunMgr, writeUntilEnd) {
  position_data_manager_t manager = {0};
  ASSERT_EQ(sunManagerInit(&manager), OBC_ERR_CODE_SUCCESS);

  uint8_t count = 0;

  position_data_t data = {1, 1, 1, 1};
  ASSERT_EQ(sunManagerWriteData(nullptr, data), OBC_ERR_CODE_INVALID_ARG);

  while (1) {
    position_t d = (position_t)count;
    position_data_t data1 = {(julian_date_t)(count + 1), d, d, d};
    if (sunManagerWriteData(&manager, data1) != OBC_ERR_CODE_SUCCESS) break;
    count++;
  }

  ASSERT_EQ(count, ADCS_POSITION_DATA_MANAGER_SIZE);

  // Invalid JD test
  data.julianDate = 0;
  ASSERT_EQ(sunManagerWriteData(&manager, data), OBC_ERR_CODE_INVALID_ARG);
}

TEST(TestObcSunMgr, readUntilEnd) {
  // Same code as test writeUntilEnd
  position_data_manager_t manager = {0};
  ASSERT_EQ(sunManagerInit(&manager), OBC_ERR_CODE_SUCCESS);

  // Writes until the end of the manager
  uint8_t countWrite = 0;
  while (1) {
    position_t d = (position_t)countWrite;
    position_data_t data = {(julian_date_t)(countWrite + 1), d, d, d};
    if (sunManagerWriteData(&manager, data) != OBC_ERR_CODE_SUCCESS) break;
    countWrite++;
  }
  ASSERT_EQ(countWrite, ADCS_POSITION_DATA_MANAGER_SIZE);

  // Finish setup

  // Reads until the end of the manager
  uint8_t countRead = 0;
  while (1) {
    position_data_t dataRead;
    if (sunManagerReadData(&manager, &dataRead) != OBC_ERR_CODE_SUCCESS) break;
    position_t d2 = (position_t)countRead;
    position_data_t dataExpect = {(julian_date_t)(countRead + 1), d2, d2, d2};
    countRead++;
    ASSERT_TRUE(equalsPositionData(dataRead, dataExpect));
  }

  ASSERT_EQ(countRead, ADCS_POSITION_DATA_MANAGER_SIZE);

  position_data_t d;
  ASSERT_EQ(sunManagerWriteData(nullptr, d), OBC_ERR_CODE_INVALID_ARG);
}

TEST(TestObcSunMgr, readWriteTwiceLength) {
  position_data_manager_t manager = {0};
  ASSERT_EQ(sunManagerInit(&manager), OBC_ERR_CODE_SUCCESS);

  // Write and read twice the size of the data manager
  for (int i = 1; i <= ADCS_POSITION_DATA_MANAGER_SIZE * 2; ++i) {
    position_data_t dataWrite = {(julian_date_t)i, 0, 0, 0};
    ASSERT_EQ(sunManagerWriteData(&manager, dataWrite), OBC_ERR_CODE_SUCCESS);

    position_data_t dataRead;
    ASSERT_EQ(sunManagerReadData(&manager, &dataRead), OBC_ERR_CODE_SUCCESS);
    ASSERT_TRUE(equalsPositionData(dataRead, dataWrite));
  }
}

TEST(TestObcSunMgr, checkJD) {
  // Setup manager
  position_data_manager_t manager = {0};
  ASSERT_EQ(sunManagerInit(&manager), OBC_ERR_CODE_SUCCESS);

  // Fill the manager with data
  for (int i = 1; i <= ADCS_POSITION_DATA_MANAGER_SIZE; ++i) {
    position_data_t dataWrite = {(julian_date_t)i, 0, 0, 0};
    ASSERT_EQ(sunManagerWriteData(&manager, dataWrite), OBC_ERR_CODE_SUCCESS);
  }

#define TEST_LENGTH 8
  julian_date_t testData[TEST_LENGTH] = {
      0, 2, 5, ADCS_POSITION_DATA_MANAGER_SIZE, ADCS_POSITION_DATA_MANAGER_SIZE + 1, 1, -1, 6.5};
  bool testResult[TEST_LENGTH] = {false, true, true, true, false, true, false, true};

  for (int i = 0; i < TEST_LENGTH; ++i) {
    uint8_t buffer;
    ASSERT_EQ(sunManagerCheckJD(&manager, testData[i], &buffer), OBC_ERR_CODE_SUCCESS);
    ASSERT_EQ(buffer, testResult[i]);
  }

  uint8_t buffer1;
  ASSERT_EQ(sunManagerCheckJD(nullptr, 1, &buffer1), OBC_ERR_CODE_INVALID_ARG);
  ASSERT_EQ(sunManagerCheckJD(&manager, 1, nullptr), OBC_ERR_CODE_INVALID_ARG);
}

TEST(TestObcSunMgr, getPositionDataManagerFull) {
  // Setup manager
  position_data_manager_t manager = {0};
  ASSERT_EQ(sunManagerInit(&manager), OBC_ERR_CODE_SUCCESS);

  // Fill the manager with data
  for (int i = 1; i <= ADCS_POSITION_DATA_MANAGER_SIZE; ++i) {
    position_data_t dataWrite = {(julian_date_t)i, (float)i * 2, (float)i * 3, (float)-i};
    ASSERT_EQ(sunManagerWriteData(&manager, dataWrite), OBC_ERR_CODE_SUCCESS);
  }

  //   // Test data
  position_data_t data1 = {2, 4, 6, -2};
  position_data_t data2 = {5, 10, 15, -5};
  position_data_t data3 = {6.9, 13.8, 20.7, -6.9};
  position_data_t data4 = {13, 26, 39, -13};

  position_data_t dataRead;

  ASSERT_EQ(sunManagerGetPositionData(&manager, 2, &dataRead), OBC_ERR_CODE_SUCCESS);
  ASSERT_TRUE(equalsPositionData(dataRead, data1));

  ASSERT_EQ(sunManagerGetPositionData(&manager, 5, &dataRead), OBC_ERR_CODE_SUCCESS);
  ASSERT_TRUE(equalsPositionData(dataRead, data2));

  ASSERT_EQ(sunManagerGetPositionData(&manager, 6.9, &dataRead), OBC_ERR_CODE_SUCCESS);
  ASSERT_TRUE(equalsPositionData(dataRead, data3));

  ASSERT_EQ(sunManagerGetPositionData(&manager, 13, &dataRead), OBC_ERR_CODE_SUCCESS);
  ASSERT_TRUE(equalsPositionData(dataRead, data4));

  // Check null pointers, should be no change to the dataRead buffer
  ASSERT_EQ(sunManagerGetPositionData(nullptr, 12, &dataRead), OBC_ERR_CODE_INVALID_ARG);
  ASSERT_TRUE(equalsPositionData(dataRead, data4));
  ASSERT_EQ(sunManagerGetPositionData(&manager, 12, nullptr), OBC_ERR_CODE_INVALID_ARG);
}

TEST(TestObcSunMgr, getPositionDataManagerPartial) {
  // Setup manager
  position_data_manager_t manager = {0};
  ASSERT_EQ(sunManagerInit(&manager), OBC_ERR_CODE_SUCCESS);

  // Fill the manager with 13 data points
  for (int i = 1; i <= 13; ++i) {
    position_data_t dataWrite = {(julian_date_t)i, (float)i * 2, (float)i * 3, (float)-i};
    ASSERT_EQ(sunManagerWriteData(&manager, dataWrite), OBC_ERR_CODE_SUCCESS);
  }

  // Test data
  position_data_t data1 = {2, 4, 6, -2};
  position_data_t data2 = {5, 10, 15, -5};
  position_data_t data3 = {6.9, 13.8, 20.7, -6.9};
  position_data_t data4 = {13, 26, 39, -13};

  position_data_t dataRead;

  ASSERT_EQ(sunManagerGetPositionData(&manager, 2, &dataRead), OBC_ERR_CODE_SUCCESS);
  ASSERT_TRUE(equalsPositionData(dataRead, data1));

  ASSERT_EQ(sunManagerGetPositionData(&manager, 5, &dataRead), OBC_ERR_CODE_SUCCESS);
  ASSERT_TRUE(equalsPositionData(dataRead, data2));

  ASSERT_EQ(sunManagerGetPositionData(&manager, 6.9, &dataRead), OBC_ERR_CODE_SUCCESS);
  ASSERT_TRUE(equalsPositionData(dataRead, data3));

  ASSERT_EQ(sunManagerGetPositionData(&manager, 13, &dataRead), OBC_ERR_CODE_SUCCESS);
  ASSERT_TRUE(equalsPositionData(dataRead, data4));
}

TEST(TestObcSunMgr, getPositionDataManagerOutOfRange) {
  // Setup manager
  position_data_manager_t manager = {0};
  ASSERT_EQ(sunManagerInit(&manager), OBC_ERR_CODE_SUCCESS);

  // Fill the manager with data
  for (int i = 1; i <= ADCS_POSITION_DATA_MANAGER_SIZE; ++i) {
    position_data_t dataWrite = {(julian_date_t)i, 0, 0, 0};
    ASSERT_EQ(sunManagerWriteData(&manager, dataWrite), OBC_ERR_CODE_SUCCESS);
  }

  // Shift manager move by 2
  for (int i = 1; i <= 2; ++i) {
    position_data_t dataRead;
    ASSERT_EQ(sunManagerReadData(&manager, &dataRead), OBC_ERR_CODE_SUCCESS);

    position_data_t dataWrite = {(julian_date_t)(i + ADCS_POSITION_DATA_MANAGER_SIZE), 0, 0, 0};
    ASSERT_EQ(sunManagerWriteData(&manager, dataWrite), OBC_ERR_CODE_SUCCESS);
  }

  position_data_t buffer;
  // Smaller
  ASSERT_EQ(sunManagerGetPositionData(&manager, 1, &buffer), OBC_ERR_CODE_SUN_POSITION_JD_OUT_OF_RANGE);

  // Larger
  ASSERT_EQ(sunManagerGetPositionData(&manager, ADCS_POSITION_DATA_MANAGER_SIZE * 2, &buffer),
            OBC_ERR_CODE_SUN_POSITION_JD_OUT_OF_RANGE);
}
