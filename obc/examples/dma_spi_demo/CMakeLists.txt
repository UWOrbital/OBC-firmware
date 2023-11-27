cmake_minimum_required (VERSION 3.15)

# includes definitions for build variables
include (${CMAKE_SOURCE_DIR}/cmake/obc_build_options.cmake)

# gcc compiler flags
set (CMAKE_C_FLAGS "-g -gdwarf-3 -gstrict-dwarf
		    -Wall -specs=\"nosys.specs\"
		    -MMD -std=gnu99")

add_executable (dma-spi-demo.out main.c)

target_include_directories(dma-spi-demo.out PRIVATE )
