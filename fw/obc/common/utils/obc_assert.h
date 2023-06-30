#pragma once

#define STATIC_ASSERT(COND, MSG) _Static_assert(COND, MSG)

#define STATIC_ASSERT_EQ(A, B) STATIC_ASSERT((A) == (B), #A " does not equal " #B)
#define STATIC_ASSERT_INT_EQ(A, B) STATIC_ASSERT((int)(A) == (int)(B), #A " does not equal " #B)
