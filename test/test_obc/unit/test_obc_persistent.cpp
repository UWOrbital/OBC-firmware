#include "obc_errors.h"
#include "obc_time_utils.h"  // Use as test subject
#include "obc_persistent.h"

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
