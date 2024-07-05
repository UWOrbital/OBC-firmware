cmake_minimum_required(VERSION 3.15)

find_package(pybind11 REQUIRED)

pybind11_add_module(log_module bindings.cpp)

target_include_directories(log_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/logsink
)

target_compile_options(log_module PUBLIC -Wall -g -pthread)

target_link_libraries(log_module PUBLIC wiringPi)

add_subdirectory(logsink)
