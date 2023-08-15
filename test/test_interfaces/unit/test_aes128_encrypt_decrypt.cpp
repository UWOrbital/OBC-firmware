#include "obc_gs_aes128.h"
#include "obc_gs_errors.h"

#include <gtest/gtest.h>

TEST(AES128, testingNullValues) {
    const uint8_t key = 0;
    obc_gs_error_code_t errCode = initializeAesCtx(NULL);
    ASSERT_EQ(errCode, OBC_GS_ERR_CODE_INVALID_ARG);

}