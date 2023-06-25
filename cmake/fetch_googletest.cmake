cmake_minimum_required(VERSION 3.15)

# ---------------------------------------------------------------
# Fetch GTest from github
# ---------------------------------------------------------------
include(FetchContent)

FetchContent_Declare(
googletest
GIT_REPOSITORY https://github.com/google/googletest.git
GIT_TAG        release-1.11.0
)
FetchContent_MakeAvailable(googletest)
add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)
# ---------------------------------------------------------------
