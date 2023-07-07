set(TEST_BINARY OBC-firmware-tests)

set(TEST_DEPENDENCIES
    ${CMAKE_SOURCE_DIR}/common/source/mock_obc_logging.c
    ${CMAKE_SOURCE_DIR}/common/source/obc_pack_utils.c
    ${CMAKE_SOURCE_DIR}/common/source/obc_unpack_utils.c
    ${CMAKE_SOURCE_DIR}/cdh/source/command_pack.c
    ${CMAKE_SOURCE_DIR}/cdh/source/command_unpack.c
    ${CMAKE_SOURCE_DIR}/comms/source/ax25.c
)

set(TEST_MOCKS

)

set(TEST_SOURCES
    ${CMAKE_SOURCE_DIR}/test/main.cpp
    ${CMAKE_SOURCE_DIR}/test/test_pack_unpack_utils.cpp
    ${CMAKE_SOURCE_DIR}/test/test_command_pack_unpack.cpp
    ${CMAKE_SOURCE_DIR}/test/test_ax25.cpp
)

set(TEST_SOURCES ${TEST_SOURCES} ${TEST_DEPENDENCIES} ${TEST_MOCKS})

add_executable(${TEST_BINARY} ${TEST_SOURCES})

target_include_directories(${TEST_BINARY}
    PRIVATE
    ${CMAKE_SOURCE_DIR}/common/include
    ${CMAKE_SOURCE_DIR}/cdh/include
    ${CMAKE_SOURCE_DIR}/comms/include
    ${CMAKE_SOURCE_DIR}/libs/libcorrect/include
)

target_link_libraries(${TEST_BINARY}
    PRIVATE
    GTest::GTest
)

add_test(${TEST_BINARY} ${TEST_BINARY})
