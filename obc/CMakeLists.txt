cmake_minimum_required(VERSION 3.15)

# Set default values for firmware build options
include(${CMAKE_SOURCE_DIR}/cmake/obc_build_options.cmake)

include(${CMAKE_CURRENT_SOURCE_DIR}/shared/hal/cmake/board_lib_defs.cmake)

# Load setup-specific definitions
include(${CMAKE_SOURCE_DIR}/obc/shared/config/peripheral_config_definitions.cmake)

set(LD_DIR_PATH "${CMAKE_CURRENT_SOURCE_DIR}/shared/hal/common/ld")
set(APP_LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/shared/hal/common/ld/app_link.ld")
set(BL_LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/shared/hal/common/ld/bl_link.ld")

if (DEBUG MATCHES 1)
    set(TARGET_OPTIMIZATION_FLAGS "-Og" "-g" "-gdwarf-3" "-gstrict-dwarf")
else()
    set(TARGET_OPTIMIZATION_FLAGS "-O2")
endif()

# Define the bootloader bypass option
option(ENABLE_BL_BYPASS "Write directly to 0x0 and overwrite the bootloader" ON)

# Set the custom address based on the option
if(ENABLE_BL_BYPASS)
    set(CUSTOM_START_ADDRESS 0)
else()
    set(CUSTOM_START_ADDRESS 0x00040000)
endif()

# Pass the address to the linker
add_link_options(-Wl,--defsym,CUSTOM_START_ADDRESS=${CUSTOM_START_ADDRESS})

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
    LOG_DATE_TIME
    DEBUG=${DEBUG}
    CMD_POLICY=${CMD_POLICY}
    COMMS_PHY=${COMMS_PHY}
    OBC_UART_BAUD_RATE=${OBC_UART_BAUD_RATE}
    CSDC_DEMO_ENABLED=${CSDC_DEMO_ENABLED}
    ENABLE_TASK_STATS_COLLECTOR=${ENABLE_TASK_STATS_COLLECTOR}
    ${PERIPHERAL_CONFIG}
)

# Determine the root of the repository
execute_process(
    COMMAND git rev-parse --show-toplevel
    OUTPUT_VARIABLE REPO_ROOT
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Set the SOURCE_PATH using the detected REPO_ROOT
add_compile_definitions(SOURCE_PATH="${REPO_ROOT}/")

target_include_directories(OBC-firmware.out PUBLIC
    ${CMAKE_BINARY_DIR}/toolchain/arm-none-eabi/include
)

add_subdirectory(app/hal)
add_subdirectory(app/modules)
add_subdirectory(app/rtos)
add_subdirectory(app/sys)
add_subdirectory(app/drivers)
add_subdirectory(app/reliance_edge)

target_link_libraries(OBC-firmware.out PRIVATE
    tiny-aes
    lib-correct
    obc-gs-interface
    attitude-control
    attitude-determination
    onboard-env-modelling
    ${HAL_LIB_OPTIMIZE}
    $<TARGET_OBJECTS:${HAL_LIB_NO_OPTIMIZE}>
)

# Generate .bin for the app
add_custom_command(
    OUTPUT OBC-firmware.bin
    COMMAND toolchain/bin/arm-none-eabi-objcopy -O binary --only-section=.intvecs
                                                                     --only-section=.kernelTEXT
                                                                     --only-section=.text
                                                                     --only-section=.rodata
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
    ${PERIPHERAL_CONFIG}
)

add_subdirectory(bl)

target_link_libraries(OBC-bl.out PRIVATE
    ${HAL_LIB_OPTIMIZE}
    $<TARGET_OBJECTS:${HAL_LIB_NO_OPTIMIZE}>
    f021_api_archive
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
    LOG_DATE_TIME
    DEBUG=${DEBUG}
    CMD_POLICY=${CMD_POLICY}
    COMMS_PHY=${COMMS_PHY}
    OBC_UART_BAUD_RATE=${OBC_UART_BAUD_RATE}
    CSDC_DEMO_ENABLED=${CSDC_DEMO_ENABLED}
    ENABLE_TASK_STATS_COLLECTOR=${ENABLE_TASK_STATS_COLLECTOR}
    ${PERIPHERAL_CONFIG}
)

add_subdirectory(app/tools/interface_debug_tool)

target_link_libraries(debug-tool.out PRIVATE
    tiny-aes
    lib-correct
    obc-gs-interface
    attitude-control
    attitude-determination
    onboard-env-modelling
    ${HAL_LIB_OPTIMIZE}
    $<TARGET_OBJECTS:${HAL_LIB_NO_OPTIMIZE}>
)
