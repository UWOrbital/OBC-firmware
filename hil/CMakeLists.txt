cmake_minimum_required(VERSION 3.15)

set(SOURCES
    hil_main.cpp
)

add_executable(hil.out ${SOURCES})

target_include_directories(hil.out PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/logsink
)

target_compile_options(hil.out PUBLIC -Wall -g -pthread)

target_link_libraries(hil.out PUBLIC wiringPi gtest gtest_main)

add_subdirectory(logsink)

add_subdirectory(examples)