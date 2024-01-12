#include "sun_mgr.h"
#include <stdio.h>

#include "obc_errors.h"

#include <gtest/gtest.h>

// Sun position data taken from the ephemeris script for Jan 1, 2024-Feb 1,2024 with a step of 1 day
static position_t sunData[][3] = {
    {2.481099325965390E+07, -1.449948612736719E+08, 8.215203670851886E+03},
    {2.738540506467866E+07, -1.445294509332678E+08, 8.105178271017969E+03},
    {2.995151006643089E+07, -1.440191068776783E+08, 7.981922637745738E+03},
    {3.250851613065001E+07, -1.434639420548778E+08, 7.849023218974471E+03},
    {3.505562589713154E+07, -1.428640828884797E+08, 7.710428671196103E+03},
    {3.759203649059476E+07, -1.422196700690944E+08, 7.570391051031649E+03},
    {4.011693897071718E+07, -1.415308596885310E+08, 7.433385715350509E+03},
    {4.262951770191272E+07, -1.407978250027577E+08, 7.303980756700039E+03},
    {4.512895002102718E+07, -1.400207590439128E+08, 7.186626320458949E+03},
    {4.761440677884826E+07, -1.391998780971540E+08, 7.085345311701298E+03},
    {5.008505441635529E+07, -1.383354256804865E+08, 7.003336033582687E+03},
    {5.254005904386971E+07, -1.374276761875439E+08, 6.942543079264462E+03},
    {5.497859242143059E+07, -1.364769370153979E+08, 6.903298163719475E+03},
    {5.739983893907548E+07, -1.354835481241328E+08, 6.884146157555282E+03},
    {5.980300210043009E+07, -1.344478786868921E+08, 6.881929896049201E+03},
    {6.218730905825550E+07, -1.333703214816240E+08, 6.892122920729220E+03},
    {6.455201245998429E+07, -1.322512863702387E+08, 6.909319823943079E+03},
    {6.689638978470010E+07, -1.310911942406353E+08, 6.927763512499630E+03},
    {6.921974096828160E+07, -1.298904722769730E+08, 6.941812758982182E+03},
    {7.152138522739623E+07, -1.286495507879745E+08, 6.946303062759340E+03},
    {7.380065775917526E+07, -1.273688613902972E+08, 6.936797042690217E+03},
    {7.605690667094061E+07, -1.260488361943869E+08, 6.909742819972336E+03},
    {7.828949025478733E+07, -1.246899076836288E+08, 6.862562496669590E+03},
    {8.049777461091174E+07, -1.232925090879427E+08, 6.793687053129077E+03},
    {8.268113160934229E+07, -1.218570751435625E+08, 6.702546976886690E+03},
    {8.483893720643054E+07, -1.203840431682142E+08, 6.589524141423404E+03},
    {8.697057015299645E+07, -1.188738543717089E+08, 6.455870720483363E+03},
    {8.907541112078135E+07, -1.173269552931376E+08, 6.303603695310652E+03},
    {9.115284223237608E+07, -1.157437992370691E+08, 6.135386305548251E+03},
    {9.320224692361164E+07, -1.141248475914762E+08, 5.954408517204225E+03},
    {9.522301001830195E+07, -1.124705709530460E+08, 5.764276322305202E+03},
};

TEST(TestObcSunMgr, writeUntilEnd) {
  position_data_manager_t manager = {0};
  ASSERT_EQ(sunManagerInit(&manager), OBC_ERR_CODE_SUCCESS);

  uint8_t count = 0;

  position_data_t data = {1, 1, 1, 1};
  ASSERT_EQ(sunManagerWriteData(nullptr, &data), OBC_ERR_CODE_INVALID_ARG);
  ASSERT_EQ(sunManagerWriteData(&manager, nullptr), OBC_ERR_CODE_INVALID_ARG);

  while (1) {
    position_t d = (position_t)count;
    position_data_t data1 = {(julian_date_t)(count + 1), d, d, d};
    if (sunManagerWriteData(&manager, &data1) != OBC_ERR_CODE_SUCCESS) break;
    count++;
  }

  ASSERT_EQ(count, ADCS_POSITION_DATA_MANAGER_SIZE);

  // Invalid JD test
  data.julianDate = 0;
  ASSERT_EQ(sunManagerWriteData(&manager, &data), OBC_ERR_CODE_INVALID_ARG);
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
    if (sunManagerWriteData(&manager, &data) != OBC_ERR_CODE_SUCCESS) break;
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
    EXPECT_TRUE(equalsPositionData(dataRead, dataExpect));
  }

  ASSERT_EQ(countRead, ADCS_POSITION_DATA_MANAGER_SIZE);

  position_data_t d;
  ASSERT_EQ(sunManagerWriteData(nullptr, &d), OBC_ERR_CODE_INVALID_ARG);
  ASSERT_EQ(sunManagerWriteData(&manager, nullptr), OBC_ERR_CODE_INVALID_ARG);
}

TEST(TestObcSunMgr, readWriteTwiceLength) {
  position_data_manager_t manager = {0};
  ASSERT_EQ(sunManagerInit(&manager), OBC_ERR_CODE_SUCCESS);

  // Write and read twice the size of the data manager
  for (int i = 1; i <= ADCS_POSITION_DATA_MANAGER_SIZE * 2; ++i) {
    position_data_t dataWrite = {(julian_date_t)i, 0, 0, 0};
    ASSERT_EQ(sunManagerWriteData(&manager, &dataWrite), OBC_ERR_CODE_SUCCESS);

    position_data_t dataRead;
    ASSERT_EQ(sunManagerReadData(&manager, &dataRead), OBC_ERR_CODE_SUCCESS);
    EXPECT_TRUE(equalsPositionData(dataRead, dataWrite));
  }
}

TEST(TestObcSunMgr, checkJD) {
  // Setup manager
  position_data_manager_t manager = {0};
  ASSERT_EQ(sunManagerInit(&manager), OBC_ERR_CODE_SUCCESS);

  // Fill the manager with data
  for (int i = 1; i <= ADCS_POSITION_DATA_MANAGER_SIZE; ++i) {
    position_data_t dataWrite = {(julian_date_t)i, 0, 0, 0};
    ASSERT_EQ(sunManagerWriteData(&manager, &dataWrite), OBC_ERR_CODE_SUCCESS);
  }

#define TEST_LENGTH 8
  julian_date_t testData[TEST_LENGTH] = {
      0, 2, 5, ADCS_POSITION_DATA_MANAGER_SIZE, ADCS_POSITION_DATA_MANAGER_SIZE + 1, 1, -1, 6.5};
  bool testResult[TEST_LENGTH] = {false, true, true, true, false, true, false, true};

  for (int i = 0; i < TEST_LENGTH; ++i) {
    bool buffer;
    ASSERT_EQ(sunManagerCheckJD(&manager, testData[i], &buffer), OBC_ERR_CODE_SUCCESS);
    EXPECT_EQ(buffer, testResult[i]);
  }

  bool buffer1;
  ASSERT_EQ(sunManagerCheckJD(nullptr, 1, &buffer1), OBC_ERR_CODE_INVALID_ARG);
  ASSERT_EQ(sunManagerCheckJD(&manager, 1, nullptr), OBC_ERR_CODE_INVALID_ARG);
}

TEST(TestObcSunMgr, getPositionDataManagerFull) {
  // Setup manager
  position_data_manager_t manager = {0};
  ASSERT_EQ(sunManagerInit(&manager), OBC_ERR_CODE_SUCCESS);

  // Fill the manager with data
  for (int i = 1; i <= ADCS_POSITION_DATA_MANAGER_SIZE; ++i) {
    position_data_t dataWrite = {(julian_date_t)i, sunData[i - 1][0], sunData[i - 1][1], sunData[i - 1][2]};
    ASSERT_EQ(sunManagerWriteData(&manager, &dataWrite), OBC_ERR_CODE_SUCCESS);
  }

  //   // Test data
  position_data_t data1 = {2, sunData[1][0], sunData[1][1], sunData[1][2]};
  position_data_t data2 = {15, sunData[14][0], sunData[14][1], sunData[14][2]};
  position_data_t data3 = {6.9, 39864448.7227, -1415999740.727, 7447.80624892};
  position_data_t data4 = {13, sunData[12][0], sunData[12][1], sunData[12][2]};
  position_data_t dataRead;

  ASSERT_EQ(sunManagerGetPositionData(&manager, 2, &dataRead), OBC_ERR_CODE_SUCCESS);
  EXPECT_TRUE(closePositionData(dataRead, data1));

  ASSERT_EQ(sunManagerGetPositionData(&manager, 15, &dataRead), OBC_ERR_CODE_SUCCESS);
  EXPECT_TRUE(closePositionData(dataRead, data2));

  ASSERT_EQ(sunManagerGetPositionData(&manager, 6.9, &dataRead), OBC_ERR_CODE_SUCCESS);
  EXPECT_TRUE(closePositionData(dataRead, data3));

  ASSERT_EQ(sunManagerGetPositionData(&manager, 13, &dataRead), OBC_ERR_CODE_SUCCESS);
  EXPECT_TRUE(closePositionData(dataRead, data4));

  // Check null pointers, should be no change to the dataRead buffer
  ASSERT_EQ(sunManagerGetPositionData(nullptr, 12, &dataRead), OBC_ERR_CODE_INVALID_ARG);
  EXPECT_TRUE(closePositionData(dataRead, data4));
  ASSERT_EQ(sunManagerGetPositionData(&manager, 12, nullptr), OBC_ERR_CODE_INVALID_ARG);
}

TEST(TestObcSunMgr, getPositionDataManagerPartial) {
  // Setup manager
  position_data_manager_t manager = {0};
  ASSERT_EQ(sunManagerInit(&manager), OBC_ERR_CODE_SUCCESS);

  // Fill the manager with 13 data points
  for (int i = 1; i <= 13; ++i) {
    position_data_t dataWrite = {(julian_date_t)i, sunData[i - 1][0], sunData[i - 1][1], sunData[i - 1][2]};
    ASSERT_EQ(sunManagerWriteData(&manager, &dataWrite), OBC_ERR_CODE_SUCCESS);
  }

  //   // Test data
  position_data_t data1 = {2, sunData[1][0], sunData[1][1], sunData[1][2]};
  position_data_t data2 = {5, sunData[4][0], sunData[4][1], sunData[4][2]};
  position_data_t data3 = {6.9, 39864448.7227, -1415999740.727, 7447.80624892};
  position_data_t data4 = {13, sunData[12][0], sunData[12][1], sunData[12][2]};

  position_data_t dataRead;

  ASSERT_EQ(sunManagerGetPositionData(&manager, 2, &dataRead), OBC_ERR_CODE_SUCCESS);
  EXPECT_TRUE(closePositionData(dataRead, data1));

  ASSERT_EQ(sunManagerGetPositionData(&manager, 5, &dataRead), OBC_ERR_CODE_SUCCESS);
  EXPECT_TRUE(closePositionData(dataRead, data2));

  ASSERT_EQ(sunManagerGetPositionData(&manager, 6.9, &dataRead), OBC_ERR_CODE_SUCCESS);
  EXPECT_TRUE(closePositionData(dataRead, data3));

  ASSERT_EQ(sunManagerGetPositionData(&manager, 13, &dataRead), OBC_ERR_CODE_SUCCESS);
  EXPECT_TRUE(closePositionData(dataRead, data4));
}

TEST(TestObcSunMgr, getPositionDataManagerOutOfRange) {
  // Setup manager
  position_data_manager_t manager = {0};
  ASSERT_EQ(sunManagerInit(&manager), OBC_ERR_CODE_SUCCESS);

  // Fill the manager with data
  for (int i = 1; i <= ADCS_POSITION_DATA_MANAGER_SIZE; ++i) {
    position_data_t dataWrite = {(julian_date_t)i, 0, 0, 0};
    ASSERT_EQ(sunManagerWriteData(&manager, &dataWrite), OBC_ERR_CODE_SUCCESS);
  }

  // Shift manager move by 2
  for (int i = 1; i <= 2; ++i) {
    position_data_t dataRead;
    ASSERT_EQ(sunManagerReadData(&manager, &dataRead), OBC_ERR_CODE_SUCCESS);

    position_data_t dataWrite = {(julian_date_t)(i + ADCS_POSITION_DATA_MANAGER_SIZE), 0, 0, 0};
    ASSERT_EQ(sunManagerWriteData(&manager, &dataWrite), OBC_ERR_CODE_SUCCESS);
  }

  position_data_t buffer;
  // Smaller
  ASSERT_EQ(sunManagerGetPositionData(&manager, 1, &buffer), OBC_ERR_CODE_SUN_POSITION_JD_OUT_OF_RANGE);

  // Larger
  ASSERT_EQ(sunManagerGetPositionData(&manager, ADCS_POSITION_DATA_MANAGER_SIZE * 2, &buffer),
            OBC_ERR_CODE_SUN_POSITION_JD_OUT_OF_RANGE);
}
