cmake_minimum_required(VERSION 3.15)

# Set default values for firmware build options
include(${CMAKE_SOURCE_DIR}/cmake/obc_build_options.cmake)

include(hal/cmake/board_lib_defs.cmake)

if(${BOARD_TYPE} MATCHES RM46_LAUNCHPAD)
    set(LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/hal/launchpad/source/sys_link.ld")
elseif(${BOARD_TYPE} MATCHES OBC_REVISION_1)
    set(LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/hal/obc_rev1/source/sys_link.ld")
elseif(${BOARD_TYPE} MATCHES OBC_REVISION_2)
    set(LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/hal/obc_rev2/source/sys_link.ld")
else()
    message(FATAL_ERROR "Invalid board type: ${BOARD_TYPE}")
endif()

add_executable(OBC-firmware.out main.c)

if (DEBUG MATCHES 1)
    target_compile_options(OBC-firmware.out PRIVATE -Og -g -gdwarf-3 -gstrict-dwarf)
else()
    target_compile_options(OBC-firmware.out PRIVATE -O2)
endif()

target_compile_definitions(OBC-firmware.out PRIVATE
    ${BOARD_TYPE}
    LOG_DEFAULT_OUTPUT_LOCATION=${LOG_DEFAULT_OUTPUT_LOCATION}
    LOG_DEFAULT_LEVEL=${LOG_DEFAULT_LEVEL}
    DEBUG=${DEBUG}
    CMD_POLICY=${CMD_POLICY}
    COMMS_PHY=${COMMS_PHY}
    OBC_UART_BAUD_RATE=${OBC_UART_BAUD_RATE}
    CSDC_DEMO_ENABLED=${CSDC_DEMO_ENABLED}
)

target_include_directories(OBC-firmware.out PUBLIC
    ${CMAKE_BINARY_DIR}/toolchain/arm-none-eabi/include
    ${CMAKE_CURRENT_SOURCE_DIR}/config
)

add_subdirectory(hal)
add_subdirectory(drivers)
add_subdirectory(sys)
add_subdirectory(modules)
add_subdirectory(reliance_edge)

target_link_libraries(OBC-firmware.out PRIVATE
    tiny-aes
    lib-correct
    obc-gs-interface
    ${HAL_LIB_ARCHIVE}
    $<TARGET_OBJECTS:${HAL_LIB_NO_OPTIMIZE}>
)
