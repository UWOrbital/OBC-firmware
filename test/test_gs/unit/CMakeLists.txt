set(TEST_BINARY gs-tests)

set(GCC_32BIT_COMPILE_OPTION "-m32")

set(TEST_DEPENDENCIES

)

set(TEST_MOCKS

)

set(TEST_SOURCES
    main.cpp
)

set(TEST_SOURCES ${TEST_SOURCES} ${TEST_DEPENDENCIES} ${TEST_MOCKS})

add_executable(${TEST_BINARY} ${TEST_SOURCES})

target_compile_options (${TEST_BINARY} PRIVATE GCC_32BIT_COMPILE_OPTION)

target_include_directories(${TEST_BINARY}
    PRIVATE

)

target_link_libraries(${TEST_BINARY}
    PRIVATE
    GTest::GTest
)

add_test(${TEST_BINARY} ${TEST_BINARY})
