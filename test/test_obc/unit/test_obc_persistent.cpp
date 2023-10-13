#include "obc_persistent.h"

#include "obc_errors.h"
#include "obc_time_utils.h"  // Use as test subject
#include "fm25v20a.h"

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
