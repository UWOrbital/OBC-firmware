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
if(${CMAKE_BUILD_TYPE} MATCHES Test)
target_compile_options(gtest INTERFACE -m32)
target_compile_options(gtest_main INTERFACE -m32)
target_compile_options(GTest::GTest INTERFACE -m32)
target_compile_options(googletest INTERFACE -m32)
endif()
target_link_libraries(GTest::GTest INTERFACE gtest_main)
# ---------------------------------------------------------------
