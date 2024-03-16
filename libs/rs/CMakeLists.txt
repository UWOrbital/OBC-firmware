cmake_minimum_required(VERSION 3.15)

set(RS_LIB rs-lib)

if (${CMAKE_BUILD_TYPE} MATCHES GS)
  add_library(${RS_LIB} SHARED 
      source/rs_wrapper.cpp
  )
else()
  add_library(${RS_LIB} STATIC 
      source/rs_wrapper.cpp
  )
endif()
target_include_directories(${RS_LIB} PUBLIC
    include
)
