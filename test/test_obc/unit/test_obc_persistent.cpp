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

  // Test setPersistentSection
  ASSERT_EQ(setPersistentSection(OBC_PERSIST_SECTION_ID_OBC_TIME, nullptr, sizeof(obc_time_persist_data_t)),
            OBC_ERR_CODE_INVALID_ARG);
  ASSERT_EQ(setPersistentSection(OBC_PERSIST_SECTION_ID_COUNT, (uint8_t *)&timeData, sizeof(obc_time_persist_data_t)),
            OBC_ERR_CODE_INVALID_ARG);
  ASSERT_EQ(setPersistentSection(OBC_PERSIST_SECTION_ID_OBC_TIME, (uint8_t *)&timeData, 0),
            OBC_ERR_CODE_BUFF_TOO_SMALL);

  // Test getPersistentSection
  ASSERT_EQ(getPersistentSection(OBC_PERSIST_SECTION_ID_OBC_TIME, nullptr, sizeof(obc_time_persist_data_t)),
            OBC_ERR_CODE_INVALID_ARG);
  ASSERT_EQ(getPersistentSection(OBC_PERSIST_SECTION_ID_COUNT, (uint8_t *)&timeData, sizeof(obc_time_persist_data_t)),
            OBC_ERR_CODE_INVALID_ARG);
  ASSERT_EQ(getPersistentSection(OBC_PERSIST_SECTION_ID_OBC_TIME, (uint8_t *)&timeData, 0),
            OBC_ERR_CODE_BUFF_TOO_SMALL);

  // Test getPersistentSectionBySubIndex
  ASSERT_EQ(
      getPersistentSectionBySubIndex(OBC_PERSIST_SECTION_ID_OBC_TIME, 0, nullptr, sizeof(obc_time_persist_data_t)),
      OBC_ERR_CODE_INVALID_ARG);
  ASSERT_EQ(getPersistentSectionBySubIndex(OBC_PERSIST_SECTION_ID_OBC_TIME, 1, (uint8_t *)&timeData,
                                           sizeof(obc_time_persist_data_t)),
            OBC_ERR_CODE_INVALID_ARG);  // Only 1 subindex
  ASSERT_EQ(getPersistentSectionBySubIndex(OBC_PERSIST_SECTION_ID_COUNT, 0, (uint8_t *)&timeData,
                                           sizeof(obc_time_persist_data_t)),
            OBC_ERR_CODE_INVALID_ARG);
  ASSERT_EQ(getPersistentSectionBySubIndex(OBC_PERSIST_SECTION_ID_OBC_TIME, 0, (uint8_t *)&timeData, 0),
            OBC_ERR_CODE_BUFF_TOO_SMALL);

  // Test setPersistentSectionBySubIndex
  ASSERT_EQ(
      setPersistentSectionBySubIndex(OBC_PERSIST_SECTION_ID_OBC_TIME, 0, nullptr, sizeof(obc_time_persist_data_t)),
      OBC_ERR_CODE_INVALID_ARG);
  ASSERT_EQ(setPersistentSectionBySubIndex(OBC_PERSIST_SECTION_ID_OBC_TIME, 1, (uint8_t *)&timeData,
                                           sizeof(obc_time_persist_data_t)),
            OBC_ERR_CODE_INVALID_ARG);  // Only 1 subindex
  ASSERT_EQ(setPersistentSectionBySubIndex(OBC_PERSIST_SECTION_ID_COUNT, 0, (uint8_t *)&timeData,
                                           sizeof(obc_time_persist_data_t)),
            OBC_ERR_CODE_INVALID_ARG);
  ASSERT_EQ(setPersistentSectionBySubIndex(OBC_PERSIST_SECTION_ID_OBC_TIME, 0, (uint8_t *)&timeData, 0),
            OBC_ERR_CODE_BUFF_TOO_SMALL);
}

TEST(TestOBCPersistent, ValidOBCTime) {
  const uint32_t UNIX_TTIME = 0x12345678;
  obc_time_persist_data_t timeData = {0};
  timeData.unixTime = UNIX_TTIME;

  // Test setPersistentSection
  ASSERT_EQ(
      setPersistentSection(OBC_PERSIST_SECTION_ID_OBC_TIME, (uint8_t *)&timeData, sizeof(obc_time_persist_data_t)),
      OBC_ERR_CODE_SUCCESS);

  timeData.unixTime = 0x00;

  // Test getPersistentSection
  obc_time_persist_data_t timeData2 = {0};
  ASSERT_EQ(
      getPersistentSection(OBC_PERSIST_SECTION_ID_OBC_TIME, (uint8_t *)&timeData2, sizeof(obc_time_persist_data_t)),
      OBC_ERR_CODE_SUCCESS);

  ASSERT_EQ(UNIX_TTIME, timeData2.unixTime);
}

TEST(TestOBCPersistent, CorruptOBCTime) {
  obc_time_persist_data_t timeData = {0};
  timeData.unixTime = 0x12345678;

  // Test setPersistentSection
  ASSERT_EQ(
      setPersistentSection(OBC_PERSIST_SECTION_ID_OBC_TIME, (uint8_t *)&timeData, sizeof(obc_time_persist_data_t)),
      OBC_ERR_CODE_SUCCESS);

  // Corrupt the data
  size_t data = 0x00;
  ASSERT_EQ(framWrite(0, (uint8_t *)&data, sizeof(size_t)), OBC_ERR_CODE_SUCCESS);

  // Test getPersistentSection
  ASSERT_EQ(
      getPersistentSection(OBC_PERSIST_SECTION_ID_OBC_TIME, (uint8_t *)&timeData, sizeof(obc_time_persist_data_t)),
      OBC_ERR_CODE_PERSISTENT_CORRUPTED);
}

TEST(TestOBCPersistent, ValidAllAlarms) {
  // Write data for to all alarms
  for (uint32_t i = 0; i < OBC_PERSISTENT_MAX_ALARM_COUNT; ++i) {
    alarm_mgr_persist_data_t alarmIn = {.unixTime = i};
    ASSERT_EQ(setPersistentSectionBySubIndex(OBC_PERSIST_SECTION_ID_ALARM_MGR, i, (uint8_t *)&alarmIn,
                                             sizeof(alarm_mgr_persist_data_t)),
              OBC_ERR_CODE_SUCCESS);
  }

  // Read out all the data for the alarms
  for (int i = 0; i < OBC_PERSISTENT_MAX_ALARM_COUNT; ++i) {
    alarm_mgr_persist_data_t alarmOut = {0};
    ASSERT_EQ(getPersistentSectionBySubIndex(OBC_PERSIST_SECTION_ID_ALARM_MGR, i, (uint8_t *)&alarmOut,
                                             sizeof(alarm_mgr_persist_data_t)),
              OBC_ERR_CODE_SUCCESS);
  }

  // Read out all the alarm data backwards to double check it
  for (int i = OBC_PERSISTENT_MAX_ALARM_COUNT - 1; i >= 0; --i) {
    alarm_mgr_persist_data_t alarmOut = {0};
    ASSERT_EQ(getPersistentSectionBySubIndex(OBC_PERSIST_SECTION_ID_ALARM_MGR, i, (uint8_t *)&alarmOut,
                                             sizeof(alarm_mgr_persist_data_t)),
              OBC_ERR_CODE_SUCCESS);
  }
}

TEST(TestOBCPersistent, CorruptAllAlarms) {
  // Write data for to all alarms
  for (uint32_t i = 0; i < OBC_PERSISTENT_MAX_ALARM_COUNT; ++i) {
    alarm_mgr_persist_data_t alarmIn = {.unixTime = i};
    ASSERT_EQ(setPersistentSectionBySubIndex(OBC_PERSIST_SECTION_ID_ALARM_MGR, i, (uint8_t *)&alarmIn,
                                             sizeof(alarm_mgr_persist_data_t)),
              OBC_ERR_CODE_SUCCESS);
  }

  // Corrupt all of the alarm's unixTimes
  for (int i = 0; i < OBC_PERSISTENT_MAX_ALARM_COUNT; ++i) {
    uint32_t corrupt = 0xFFFF;
    // unixTimeAddr is equivalent to what is done in the set/get
    uint32_t unixTimeAddr = OBC_PERSIST_ADDR_OF(alarmMgr[0].data) + sizeof(alarm_mgr_persist_t) * i;
    framWrite(unixTimeAddr, (uint8_t *)&corrupt, sizeof(uint32_t));
  }

  // Read out all the data for the alarms
  for (int i = 0; i < OBC_PERSISTENT_MAX_ALARM_COUNT; ++i) {
    alarm_mgr_persist_data_t alarmOut = {0};
    ASSERT_EQ(getPersistentSectionBySubIndex(OBC_PERSIST_SECTION_ID_ALARM_MGR, i, (uint8_t *)&alarmOut,
                                             sizeof(alarm_mgr_persist_data_t)),
              OBC_ERR_CODE_PERSISTENT_CORRUPTED);
  }
}
