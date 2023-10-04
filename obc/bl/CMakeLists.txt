cmake_minimum_required(VERSION 3.15)

set(BL_BIN OBC-bl.out CACHE STRING "Name of the bootloader binary")

include(${CMAKE_SOURCE_DIR}/cmake/obc_build_options.cmake)

include(${CMAKE_SOURCE_DIR}/obc/bl/bl_hal/cmake/board_lib_defs.cmake)

set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/obc/bl/bl_link.ld")

set(BL_SOURCES
    ${CMAKE_SOURCE_DIR}/obc/bl/bl_main.c
    ${CMAKE_SOURCE_DIR}/obc/bl/bl_flash.c
)

add_executable(${BL_BIN} ${BL_SOURCES})

target_link_options(${BL_BIN} PRIVATE -specs=nosys.specs -T${LINKER_SCRIPT} -Wl,--print-memory-usage)
target_link_options(${BL_BIN}
    PUBLIC
    LINKER:-Map=${CMAKE_BINARY_DIR}/${BL_BIN}.map
)

if (DEBUG MATCHES 1)
    target_compile_options(${BL_BIN} PRIVATE -Og -g -gdwarf-3 -gstrict-dwarf)
else()
    target_compile_options(${BL_BIN} PRIVATE -O2)
endif()

target_include_directories(${BL_BIN} PUBLIC
    ${CMAKE_BINARY_DIR}/toolchain/arm-none-eabi/include
)

target_sources(${BL_BIN} PUBLIC
    ${CMAKE_SOURCE_DIR}/obc/bl/F021_Flash_API/source/Fapi_UserDefinedFunctions.c
)

add_library(f021_api STATIC IMPORTED)
set_property(TARGET f021_api PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/obc/bl/F021_Flash_API/F021_API_CortexR4_LE.lib)
target_include_directories(f021_api INTERFACE ${CMAKE_SOURCE_DIR}/obc/bl/F021_Flash_API/include)

add_library(f021_api_archive INTERFACE)
target_link_libraries(f021_api_archive INTERFACE "-Wl,--whole-archive" f021_api "-Wl,--no-whole-archive")

add_subdirectory(bl_hal)

target_link_libraries(${BL_BIN} PRIVATE
    ${HAL_LIB_OPTIMIZE}
    $<TARGET_OBJECTS:${HAL_LIB_NO_OPTIMIZE}>
    f021_api_archive
)
