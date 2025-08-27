cmake_minimum_required(VERSION 3.15)

set(ATTITUDE_DETERMINATION_LIB attitude-determination)

add_library(${ATTITUDE_DETERMINATION_LIB} STATIC
    source/attitude_determination_and_vehi.c
)

target_include_directories(${ATTITUDE_DETERMINATION_LIB} PUBLIC
    include
)
