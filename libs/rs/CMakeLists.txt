cmake_minimum_required(VERSION 3.15)

set(RS_LIB rs-lib)

add_library(${RS_LIB} ${LIBRARY_TYPE} 
      source/rs_wrapper.cpp
  )
target_include_directories(${RS_LIB} PUBLIC
    include
)
