set(TEST_BINARY OBC-firmware-tests)

set(TEST_DEPENDENCIES
    ${CMAKE_SOURCE_DIR}/fw/obc/sys/time/obc_time_utils.c
)

set(TEST_MOCKS

)

set(TEST_SOURCES
    ${CMAKE_SOURCE_DIR}/fw/obc/test/unit/main.cpp
    ${CMAKE_SOURCE_DIR}/fw/obc/test/unit/test_obc_time_utils.cpp
)

set(TEST_SOURCES ${TEST_SOURCES} ${TEST_DEPENDENCIES} ${TEST_MOCKS})

add_executable(${TEST_BINARY} ${TEST_SOURCES})

target_include_directories(${TEST_BINARY}
    PRIVATE
    ${CMAKE_SOURCE_DIR}/fw/obc/sys
    ${CMAKE_SOURCE_DIR}/fw/obc/sys/time
    ${CMAKE_SOURCE_DIR}/fw/obc/drivers/peripherals/ds3232
)

target_link_libraries(${TEST_BINARY}
    PRIVATE
    GTest::GTest
)

add_test(${TEST_BINARY} ${TEST_BINARY})
