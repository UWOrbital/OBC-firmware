cmake_minimum_required(VERSION 3.15)

set(ATTITUDE_CONTROL_LIB attitude-control)

add_library(${ATTITUDE_CONTROL_LIB} STATIC
    source/attitude_control.c 
    source/ert_main.c
)

target_include_directories(${ATTITUDE_CONTROL_LIB} PUBLIC
    include
)
