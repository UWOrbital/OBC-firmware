cmake_minimum_required(VERSION 3.15)

set(ATTITUDE_CONTROL_LIB attitude-control)

add_library(${ATTITUDE_CONTROL_LIB} STATIC
    source/attitude_control_data.c 
    source/attitude_control.c 
    source/rt_nonfinite.c 
    source/rtGetInf.c 
    source/rtGetNaN.c
    source/rt_logging.c
)

target_include_directories(${ATTITUDE_CONTROL_LIB} PUBLIC
    include
)
