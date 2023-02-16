cmake_minimum_required(VERSION 3.15)

target_include_directories(obc-unit-tests PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_sources(obc-unit-tests PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cmock.c
)