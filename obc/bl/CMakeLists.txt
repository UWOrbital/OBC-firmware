cmake_minimum_required(VERSION 3.15)

set(BL_BIN OBC-bl.out CACHE STRING "Name of the bootloader binary")

include(${CMAKE_SOURCE_DIR}/cmake/obc_build_options.cmake)

include(${CMAKE_SOURCE_DIR}/obc/bl/bl_hal/cmake/board_lib_defs.cmake)

set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/obc/bl/bl_hal/launchpad/source/sys_link.ld")

add_link_options(-specs=nosys.specs -Wl,-Map=${CMAKE_PROJECT_NAME}.map -T${LINKER_SCRIPT} -Wl,--print-memory-usage)

add_executable(${BL_BIN} ${CMAKE_SOURCE_DIR}/obc/bl/bl_main.c)

if (DEBUG MATCHES 1)
    target_compile_options(${BL_BIN} PRIVATE -Og -g -gdwarf-3 -gstrict-dwarf)
else()
    target_compile_options(${BL_BIN} PRIVATE -O2)
endif()

target_include_directories(${BL_BIN} PUBLIC
    ${CMAKE_BINARY_DIR}/toolchain/arm-none-eabi/include
    ${CMAKE_SOURCE_DIR}/obc/config
)

add_subdirectory(bl_hal)

target_link_libraries(${BL_BIN} PRIVATE
    ${BL_HAL_LIB_ARCHIVE}
    $<TARGET_OBJECTS:${BL_HAL_LIB_NO_OPTIMIZE}>
)
