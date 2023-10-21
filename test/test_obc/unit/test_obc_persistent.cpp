#include "obc_persistent.h"

#include "obc_errors.h"
#include "fm25v20a.h"

// Test subjects, add more as persistent grows
#include "obc_time_utils.h"
#include "alarm_handler.h"

#include <gtest/gtest.h>

TEST(TestOBCPersistent, InvalidArgs) {
  obc_time_persist_data_t timeData = {0};
  timeData.unixTime = 0x12345678;

  // Test setPersistentData
  ASSERT_EQ(setPersistentData(OBC_PERSIST_SECTION_ID_OBC_TIME, nullptr, sizeof(obc_time_persist_data_t)),
            OBC_ERR_CODE_INVALID_ARG);
  ASSERT_EQ(setPersistentData(OBC_PERSIST_SECTION_ID_COUNT, (uint8_t *)&timeData, sizeof(obc_time_persist_data_t)),
            OBC_ERR_CODE_INVALID_ARG);
  ASSERT_EQ(setPersistentData(OBC_PERSIST_SECTION_ID_OBC_TIME, (uint8_t *)&timeData, 0), OBC_ERR_CODE_BUFF_TOO_SMALL);

  // Test getPersistentData
  ASSERT_EQ(getPersistentData(OBC_PERSIST_SECTION_ID_OBC_TIME, nullptr, sizeof(obc_time_persist_data_t)),
            OBC_ERR_CODE_INVALID_ARG);
  ASSERT_EQ(getPersistentData(OBC_PERSIST_SECTION_ID_COUNT, (uint8_t *)&timeData, sizeof(obc_time_persist_data_t)),
            OBC_ERR_CODE_INVALID_ARG);
  ASSERT_EQ(getPersistentData(OBC_PERSIST_SECTION_ID_OBC_TIME, (uint8_t *)&timeData, 0), OBC_ERR_CODE_BUFF_TOO_SMALL);

  // Test getPersistentDataByIndex
  ASSERT_EQ(getPersistentDataByIndex(OBC_PERSIST_SECTION_ID_OBC_TIME, 0, nullptr, sizeof(obc_time_persist_data_t)),
            OBC_ERR_CODE_INVALID_ARG);
  ASSERT_EQ(getPersistentDataByIndex(OBC_PERSIST_SECTION_ID_OBC_TIME, 1, (uint8_t *)&timeData,
                                     sizeof(obc_time_persist_data_t)),
            OBC_ERR_CODE_INVALID_ARG);  // Only 1 index
  ASSERT_EQ(
      getPersistentDataByIndex(OBC_PERSIST_SECTION_ID_COUNT, 0, (uint8_t *)&timeData, sizeof(obc_time_persist_data_t)),
      OBC_ERR_CODE_INVALID_ARG);
  ASSERT_EQ(getPersistentDataByIndex(OBC_PERSIST_SECTION_ID_OBC_TIME, 0, (uint8_t *)&timeData, 0),
            OBC_ERR_CODE_BUFF_TOO_SMALL);

  // Test setPersistentDataByIndex
  ASSERT_EQ(setPersistentDataByIndex(OBC_PERSIST_SECTION_ID_OBC_TIME, 0, nullptr, sizeof(obc_time_persist_data_t)),
            OBC_ERR_CODE_INVALID_ARG);
  ASSERT_EQ(setPersistentDataByIndex(OBC_PERSIST_SECTION_ID_OBC_TIME, 1, (uint8_t *)&timeData,
                                     sizeof(obc_time_persist_data_t)),
            OBC_ERR_CODE_INVALID_ARG);  // Only 1 index
  ASSERT_EQ(
      setPersistentDataByIndex(OBC_PERSIST_SECTION_ID_COUNT, 0, (uint8_t *)&timeData, sizeof(obc_time_persist_data_t)),
      OBC_ERR_CODE_INVALID_ARG);
  ASSERT_EQ(setPersistentDataByIndex(OBC_PERSIST_SECTION_ID_OBC_TIME, 0, (uint8_t *)&timeData, 0),
            OBC_ERR_CODE_BUFF_TOO_SMALL);
}

TEST(TestOBCPersistent, ValidOBCTime) {
  const uint32_t UNIX_TTIME = 0x12345678;
  obc_time_persist_data_t timeData = {0};
  timeData.unixTime = UNIX_TTIME;

  // Test setPersistentData
  ASSERT_EQ(setPersistentData(OBC_PERSIST_SECTION_ID_OBC_TIME, (uint8_t *)&timeData, sizeof(obc_time_persist_data_t)),
            OBC_ERR_CODE_SUCCESS);

  timeData.unixTime = 0x00;

  // Test getPersistentData
  obc_time_persist_data_t timeData2 = {0};
  ASSERT_EQ(getPersistentData(OBC_PERSIST_SECTION_ID_OBC_TIME, (uint8_t *)&timeData2, sizeof(obc_time_persist_data_t)),
            OBC_ERR_CODE_SUCCESS);

  ASSERT_EQ(UNIX_TTIME, timeData2.unixTime);
}

TEST(TestOBCPersistent, CorruptOBCTime) {
  obc_time_persist_data_t timeData = {0};
  timeData.unixTime = 0x12345678;

  // Test setPersistentData
  ASSERT_EQ(setPersistentData(OBC_PERSIST_SECTION_ID_OBC_TIME, (uint8_t *)&timeData, sizeof(obc_time_persist_data_t)),
            OBC_ERR_CODE_SUCCESS);

  // Corrupt the data
  size_t data = 0x00;
  ASSERT_EQ(framWrite(0, (uint8_t *)&data, sizeof(size_t)), OBC_ERR_CODE_SUCCESS);

  // Test getPersistentData
  ASSERT_EQ(getPersistentData(OBC_PERSIST_SECTION_ID_OBC_TIME, (uint8_t *)&timeData, sizeof(obc_time_persist_data_t)),
            OBC_ERR_CODE_PERSISTENT_CORRUPTED);
}

// Writes all the data to the alarms
static void writeToAllAlarms() {
  for (uint32_t i = 0; i < OBC_PERSISTENT_MAX_ALARM_COUNT; ++i) {
    alarm_mgr_persist_data_t alarmIn = {
        .unixTime = i,
        .type = ALARM_TYPE_TIME_TAGGED_CMD,
    };
    ASSERT_EQ(setPersistentDataByIndex(OBC_PERSIST_SECTION_ID_ALARM_MGR, i, (uint8_t *)&alarmIn,
                                       sizeof(alarm_mgr_persist_data_t)),
              OBC_ERR_CODE_SUCCESS);
  }
}

// Reads all the data from the alarms
static void readAllAlarmsRegular() {
  for (uint32_t i = 0; i < OBC_PERSISTENT_MAX_ALARM_COUNT; ++i) {
    alarm_mgr_persist_data_t alarmOut = {0};
    ASSERT_EQ(getPersistentDataByIndex(OBC_PERSIST_SECTION_ID_ALARM_MGR, i, (uint8_t *)&alarmOut,
                                       sizeof(alarm_mgr_persist_data_t)),
              OBC_ERR_CODE_SUCCESS);
    ASSERT_EQ(alarmOut.unixTime, i);
    ASSERT_EQ(alarmOut.type, ALARM_TYPE_TIME_TAGGED_CMD);
  }
}

TEST(TestOBCPersistent, ValidAllAlarms) {
  writeToAllAlarms();
  readAllAlarmsRegular();

  // Read out all the alarm data backwards to double check it
  for (uint32_t i = OBC_PERSISTENT_MAX_ALARM_COUNT - 1; i >= 0; --i) {
    alarm_mgr_persist_data_t alarmOut = {0};
    ASSERT_EQ(getPersistentDataByIndex(OBC_PERSIST_SECTION_ID_ALARM_MGR, i, (uint8_t *)&alarmOut,
                                       sizeof(alarm_mgr_persist_data_t)),
              OBC_ERR_CODE_SUCCESS);
    // i-th alarm will have unixTime = i
    ASSERT_EQ(alarmOut.unixTime, i);
    ASSERT_EQ(alarmOut.type, ALARM_TYPE_TIME_TAGGED_CMD);

    if (0 == i) break;  // Otherwise it will loop around to max of uint32
  }
}

TEST(TestOBCPersistent, CorruptAllAlarms) {
  // Write data for to all alarms
  writeToAllAlarms();

  // Corrupt all of the alarm's unixTimes
  for (int i = 0; i < OBC_PERSISTENT_MAX_ALARM_COUNT; ++i) {
    uint32_t corrupt = 0xFFFF;
    // unixTimeAddr is calculated the same way as in the set/get persistent by sub index
    //  but outside of testing SHOULD NOT BE USED, use the provided functions
    uint32_t unixTimeAddr = OBC_PERSIST_ADDR_OF(alarmMgr[0].data) + sizeof(alarm_mgr_persist_t) * i;
    framWrite(unixTimeAddr, (uint8_t *)&corrupt, sizeof(uint32_t));
  }

  // Read out all the data for the alarms
  for (int i = 0; i < OBC_PERSISTENT_MAX_ALARM_COUNT; ++i) {
    alarm_mgr_persist_data_t alarmOut = {0};
    ASSERT_EQ(getPersistentDataByIndex(OBC_PERSIST_SECTION_ID_ALARM_MGR, i, (uint8_t *)&alarmOut,
                                       sizeof(alarm_mgr_persist_data_t)),
              OBC_ERR_CODE_PERSISTENT_CORRUPTED);
  }
}

TEST(TestOBCPersistent, timeAndAlarms) {
  writeToAllAlarms();
  obc_time_persist_data_t timeDataIn = {0};
  timeDataIn.unixTime = 0x12345678;

  ASSERT_EQ(setPersistentData(OBC_PERSIST_SECTION_ID_OBC_TIME, (uint8_t *)&timeDataIn, sizeof(obc_time_persist_data_t)),
            OBC_ERR_CODE_SUCCESS);

  obc_time_persist_data_t timeDataOut = {0};
  readAllAlarmsRegular();
  ASSERT_EQ(
      getPersistentData(OBC_PERSIST_SECTION_ID_OBC_TIME, (uint8_t *)&timeDataOut, sizeof(obc_time_persist_data_t)),
      OBC_ERR_CODE_SUCCESS);
  ASSERT_EQ(timeDataOut.unixTime, timeDataIn.unixTime);
}
