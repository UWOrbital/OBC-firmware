cmake_minimum_required(VERSION 3.15)

set(ATTITUDE_DETERMINATION_LIB attitude-determination)

add_library(${ATTITUDE_DETERMINATION_LIB} STATIC
    source/attitude_determination_and_vehi_data.c 
    source/attitude_determination_and_vehi.c 
    source/rt_nonfinite.c 
    source/rtGetInf.c 
    source/rtGetNaN.c
    source/rt_logging.c
)

target_include_directories(${ATTITUDE_DETERMINATION_LIB} PUBLIC
    include
)
