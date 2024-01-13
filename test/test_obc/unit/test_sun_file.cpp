#include "sun_file.h"

#include "obc_errors.h"

#include <gtest/gtest.h>

TEST(TestObcSunFile, init) { ASSERT_EQ(sunFileInit(NULL), OBC_ERR_CODE_INVALID_ARG); }
