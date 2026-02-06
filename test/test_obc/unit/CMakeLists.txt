set(TEST_BINARY obc-firmware-tests)

set(TEST_DEPENDENCIES
    ${CMAKE_SOURCE_DIR}/obc/app/sys/time/obc_time_utils.c
    ${CMAKE_SOURCE_DIR}/obc/app/drivers/arducam/image_processing.c
    ${CMAKE_SOURCE_DIR}/obc/app/drivers/vn100/vn100_binary_parsing.c
    ${CMAKE_SOURCE_DIR}/interfaces/obc_gs_interface/common/obc_gs_crc.c
    ${CMAKE_SOURCE_DIR}/interfaces/data_pack_unpack/data_unpack_utils.c
    ${CMAKE_SOURCE_DIR}/obc/app/sys/persistent/obc_persistent.c
    ${CMAKE_SOURCE_DIR}/obc/app/drivers/ina230/ina230.c
    ${CMAKE_SOURCE_DIR}/obc/app/drivers/tca6424/tca6424.c
)

set(TEST_MOCKS
    ${CMAKE_SOURCE_DIR}/test/mocks/mock_logging.c
    ${CMAKE_SOURCE_DIR}/test/mocks/mock_fram.c
    ${CMAKE_SOURCE_DIR}/test/mocks/mock_crc.c
    ${CMAKE_SOURCE_DIR}/test/mocks/mock_freertos.c
)

option(USE_MOCK_I2C "Use mock I2C for testing" ON)

if(USE_MOCK_I2C)
    list(APPEND TEST_MOCKS ${CMAKE_SOURCE_DIR}/test/mocks/mock_i2c_hal.c)
else()
    list(APPEND TEST_DEPENDENCIES ${CMAKE_SOURCE_DIR}/obc/app/drivers/rm46/obc_i2c_io.c)
endif()

set(TEST_SOURCES
    ${CMAKE_SOURCE_DIR}/test/test_obc/unit/main.cpp
    ${CMAKE_SOURCE_DIR}/test/test_obc/unit/test_obc_time_utils.cpp
    ${CMAKE_SOURCE_DIR}/test/test_obc/unit/test_image_processing.cpp
    ${CMAKE_SOURCE_DIR}/test/test_obc/unit/test_vn100_unpack.cpp
    ${CMAKE_SOURCE_DIR}/test/test_obc/unit/test_obc_persistent.cpp
    ${CMAKE_SOURCE_DIR}/test/test_obc/unit/test_ina230.cpp
)

set(TEST_SOURCES ${TEST_SOURCES} ${TEST_DEPENDENCIES} ${TEST_MOCKS})

add_executable(${TEST_BINARY} ${TEST_SOURCES})

target_include_directories(${TEST_BINARY}
    PRIVATE
    ${CMAKE_SOURCE_DIR}/obc/app/sys
    ${CMAKE_SOURCE_DIR}/obc/app/sys/time
    ${CMAKE_SOURCE_DIR}/obc/app/sys/utils
    ${CMAKE_SOURCE_DIR}/obc/app/sys/logging
    ${CMAKE_SOURCE_DIR}/obc/app/sys/persistent
    ${CMAKE_SOURCE_DIR}/obc/app/drivers/ds3232
    ${CMAKE_SOURCE_DIR}/obc/app/drivers/arducam
    ${CMAKE_SOURCE_DIR}/obc/app/drivers/vn100
    ${CMAKE_SOURCE_DIR}/interfaces/obc_gs_interface/common
    ${CMAKE_SOURCE_DIR}/interfaces/data_pack_unpack
    ${CMAKE_SOURCE_DIR}/obc/app/drivers/fram
    ${CMAKE_SOURCE_DIR}/obc/app/reliance_edge/projects/freertos_rm46/host/ # redconf.h
    ${CMAKE_SOURCE_DIR}/obc/app/reliance_edge/include # redconf.h
    ${CMAKE_SOURCE_DIR}/obc/app/modules/alarm_mgr
    ${CMAKE_SOURCE_DIR}/obc/app/modules/command_mgr
    ${CMAKE_SOURCE_DIR}/interfaces/obc_gs_interface/commands
    ${CMAKE_SOURCE_DIR}/obc/app/drivers/ina230
    ${CMAKE_SOURCE_DIR}/obc/app/drivers/rm46
    ${CMAKE_SOURCE_DIR}/obc/shared/hal/obc_rev1/include
    ${CMAKE_SOURCE_DIR}/obc/app/drivers/tca6424
    ${CMAKE_SOURCE_DIR}/test/mocks
    ${CMAKE_SOURCE_DIR}/obc/shared/obc_errors
    ${CMAKE_SOURCE_DIR}/obc/shared/logging
    ${CMAKE_SOURCE_DIR}/obc/app/sys/logging
    ${CMAKE_SOURCE_DIR}/obc/shared/hal/freertos/include
    ${CMAKE_SOURCE_DIR}/obc/shared/commands
)

target_link_libraries(${TEST_BINARY}
    PRIVATE
    GTest::GTest
)

add_test(${TEST_BINARY} ${TEST_BINARY})

if(USE_MOCK_I2C)
    target_compile_definitions(${TEST_BINARY} PRIVATE USE_MOCK_I2C)
endif()
