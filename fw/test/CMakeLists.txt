set(TEST_BINARY OBC-firmware-tests)

set(TEST_DEPENDENCIES
    ${CMAKE_SOURCE_DIR}/fw/interfaces/data_pack_unpack/data_pack_utils.c
    ${CMAKE_SOURCE_DIR}/fw/interfaces/data_pack_unpack/data_unpack_utils.c
    ${CMAKE_SOURCE_DIR}/fw/interfaces/obc_gs_interface/commands/command_pack.c
    ${CMAKE_SOURCE_DIR}/fw/interfaces/obc_gs_interface/commands/command_unpack.c
)

set(TEST_MOCKS

)

set(TEST_SOURCES
    ${CMAKE_SOURCE_DIR}/fw/test/unit/main.cpp
    ${CMAKE_SOURCE_DIR}/fw/test/unit/test_pack_unpack_utils.cpp
    ${CMAKE_SOURCE_DIR}/fw/test/unit/test_command_pack_unpack.cpp
)

set(TEST_SOURCES ${TEST_SOURCES} ${TEST_DEPENDENCIES} ${TEST_MOCKS})

add_executable(${TEST_BINARY} ${TEST_SOURCES})

target_include_directories(${TEST_BINARY}
    PRIVATE
    ${CMAKE_SOURCE_DIR}/fw/interfaces/data_pack_unpack/
    ${CMAKE_SOURCE_DIR}/fw/interfaces/obc_gs_interface/common
    ${CMAKE_SOURCE_DIR}/fw/interfaces/obc_gs_interface/commands
)

target_link_libraries(${TEST_BINARY}
    PRIVATE
    GTest::GTest
)

add_test(${TEST_BINARY} ${TEST_BINARY})
