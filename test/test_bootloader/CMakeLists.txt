set(TEST_BINARY obc-bootloader-tests)

set(TEST_DEPENDENCIES
    ${CMAKE_SOURCE_DIR}/obc/bl/source/bl_crc.c
)

set(TEST_SOURCES
    ${CMAKE_SOURCE_DIR}/test/test_bootloader/test_checksum.cpp
)

set(TEST_SOURCES ${TEST_SOURCES} ${TEST_DEPENDENCIES})

add_executable(${TEST_BINARY} ${TEST_SOURCES})

target_include_directories(${TEST_BINARY}
    PRIVATE
    ${CMAKE_SOURCE_DIR}/obc/bl/include
)

target_link_libraries(${TEST_BINARY}
    PRIVATE
    GTest::GTest
)

add_test(${TEST_BINARY} ${TEST_BINARY})
