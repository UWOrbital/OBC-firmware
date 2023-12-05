cmake_minimum_required(VERSION 3.15)

# Set default values for firmware build options
include(${CMAKE_SOURCE_DIR}/cmake/obc_build_options.cmake)

include(${CMAKE_CURRENT_SOURCE_DIR}/shared/hal/cmake/board_lib_defs.cmake)

set(LD_DIR_PATH "${CMAKE_CURRENT_SOURCE_DIR}/ld")
set(APP_LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/ld/app_link.ld")
set(BL_LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/ld/bl_link.ld")

if (DEBUG MATCHES 1)
    set(TARGET_OPTIMIZATION_FLAGS "-Og" "-g" "-gdwarf-3" "-gstrict-dwarf")
else()
    set(TARGET_OPTIMIZATION_FLAGS "-O2")
endif()

add_executable(OBC-firmware.out app/app_main.c)
add_executable(OBC-bl.out)
add_executable(debug-tool.out)

# Shared
add_subdirectory(shared/config)
add_subdirectory(shared/hal)

# Application
target_link_options(OBC-firmware.out PRIVATE
    -specs=nosys.specs
    -Wl,-Map=OBC-firmware.map -T${APP_LINKER_SCRIPT}
    -Wl,--print-memory-usage
    -Wl,-L${LD_DIR_PATH}
)

target_compile_options(OBC-firmware.out PRIVATE ${TARGET_OPTIMIZATION_FLAGS})

target_compile_definitions(OBC-firmware.out PRIVATE
    ${BOARD_TYPE}
    LOG_DEFAULT_OUTPUT_LOCATION=${LOG_DEFAULT_OUTPUT_LOCATION}
    LOG_DEFAULT_LEVEL=${LOG_DEFAULT_LEVEL}
    DEBUG=${DEBUG}
    CMD_POLICY=${CMD_POLICY}
    COMMS_PHY=${COMMS_PHY}
    OBC_UART_BAUD_RATE=${OBC_UART_BAUD_RATE}
    CSDC_DEMO_ENABLED=${CSDC_DEMO_ENABLED}
    ENABLE_TASK_STATS_COLLECTOR=${ENABLE_TASK_STATS_COLLECTOR}
)

target_include_directories(OBC-firmware.out PUBLIC
    ${CMAKE_BINARY_DIR}/toolchain/arm-none-eabi/include
)

add_subdirectory(app/modules)
add_subdirectory(app/rtos)
add_subdirectory(app/sys)
add_subdirectory(app/drivers)
add_subdirectory(app/reliance_edge)

target_link_libraries(OBC-firmware.out PRIVATE
    tiny-aes
    lib-correct
    obc-gs-interface
    ${HAL_LIB_OPTIMIZE}
    $<TARGET_OBJECTS:${HAL_LIB_NO_OPTIMIZE}>
)

# Generate .bin for the app
add_custom_command(
    OUTPUT OBC-firmware.bin
    COMMAND toolchain/bin/arm-none-eabi-objcopy -O binary --only-section=.intvecs
                                                                     --only-section=.kernelTEXT
                                                                     --only-section=.text
                                                                     --only-section=.ARM
                                                                     --only-section=.preinit_array
                                                                     --only-section=.init_array
                                                                     --only-section=.fini_array
                                                                     --only-section=.data
                                                                        OBC-firmware.out OBC-firmware.bin
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    DEPENDS OBC-firmware.out
    COMMENT "Generating OBC-firmware.bin"
)

add_custom_target(
    OBC-firmware_bin
    ALL
    DEPENDS OBC-firmware.bin
)

# Bootloader
target_link_options(OBC-bl.out PRIVATE
    -specs=nosys.specs
    -Wl,-Map=OBC-bl.map -T${BL_LINKER_SCRIPT}
    -Wl,--print-memory-usage
    -Wl,-L${LD_DIR_PATH}
)

target_compile_options(OBC-bl.out PRIVATE ${TARGET_OPTIMIZATION_FLAGS})

target_compile_definitions(OBC-bl.out PRIVATE
    ${BOARD_TYPE}
    LOG_DEFAULT_OUTPUT_LOCATION=${LOG_DEFAULT_OUTPUT_LOCATION}
    LOG_DEFAULT_LEVEL=${LOG_DEFAULT_LEVEL}
    DEBUG=${DEBUG}
    CMD_POLICY=${CMD_POLICY}
    COMMS_PHY=${COMMS_PHY}
    OBC_UART_BAUD_RATE=${OBC_UART_BAUD_RATE}
    CSDC_DEMO_ENABLED=${CSDC_DEMO_ENABLED}
    ENABLE_TASK_STATS_COLLECTOR=${ENABLE_TASK_STATS_COLLECTOR}
)

add_subdirectory(bl)

target_link_libraries(OBC-bl.out PRIVATE
    ${HAL_LIB_OPTIMIZE}
    $<TARGET_OBJECTS:${HAL_LIB_NO_OPTIMIZE}>
)

# Tools
target_link_options(debug-tool.out PRIVATE
    -specs=nosys.specs
    -Wl,-Map=debug-tool.map -T${APP_LINKER_SCRIPT}
    -Wl,-L${LD_DIR_PATH}
)

target_compile_options(debug-tool.out PRIVATE ${TARGET_OPTIMIZATION_FLAGS})

target_compile_definitions(debug-tool.out PRIVATE
    ${BOARD_TYPE}
    LOG_DEFAULT_OUTPUT_LOCATION=${LOG_DEFAULT_OUTPUT_LOCATION}
    LOG_DEFAULT_LEVEL=${LOG_DEFAULT_LEVEL}
    DEBUG=${DEBUG}
    CMD_POLICY=${CMD_POLICY}
    COMMS_PHY=${COMMS_PHY}
    OBC_UART_BAUD_RATE=${OBC_UART_BAUD_RATE}
    CSDC_DEMO_ENABLED=${CSDC_DEMO_ENABLED}
    ENABLE_TASK_STATS_COLLECTOR=${ENABLE_TASK_STATS_COLLECTOR}
)

add_subdirectory(app/tools/interface_debug_tool)

target_link_libraries(debug-tool.out PRIVATE
    tiny-aes
    lib-correct
    obc-gs-interface
    ${HAL_LIB_OPTIMIZE}
    $<TARGET_OBJECTS:${HAL_LIB_NO_OPTIMIZE}>
)
