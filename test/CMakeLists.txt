cmake_minimum_required(VERSION 3.15)

include(${CMAKE_SOURCE_DIR}/cmake/fetch_googletest.cmake)

enable_testing()

add_subdirectory(test_interfaces/unit)

add_subdirectory(test_obc/unit)
