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

if(${CMAKE_BUILD_TYPE} MATCHES OBC)
	add_executable(OBC-firmware.out main.c)
elseif(${CMAKE_BUILD_TYPE} MATCHES Examples)
	if (${EXAMPLE_TYPE} MATCHES DMA_SPI)
		add_executable(OBC-firmware.out examples/dma_spi_demo/main.c)
	elseif(${EXAMPLE_TYPE} MATCHES FRAM_PERSIST)
		add_executable(OBC-firmware.out examples/test_app_fram_persist/main.c)
	elseif(${EXAMPLE_TYPE} MATCHES FRAM_SPI)
		add_executable(OBC-firmware.out examples/test_app_fram_spi/main.c)
	elseif(${EXAMPLE_TYPE} MATCHES LM75BD)
		add_executable(OBC-firmware.out examples/test_app_lm75bd/main.c)
	elseif(${EXAMPLE_TYPE} MATCHES MPU6050)
		add_executable(OBC-firmware.out examples/test_app_mpu6050/main.c examples/test_app_mpu6050/source/mpu6050.c)
		target_include_directories (OBC-firmware.out PRIVATE examples/test_app_mpu6050/include)
	elseif(${EXAMPLE_TYPE} MATCHES RE_SD)
		add_executable(OBC-firmware.out examples/test_app_reliance_edge_sd/main.c)
	elseif(${EXAMPLE_TYPE} MATCHES RTC)
		add_executable(OBC-firmware.out examples/test_app_rtc/main.c)
	elseif(${EXAMPLE_TYPE} MATCHES UART_RX)
		add_executable(OBC-firmware.out examples/test_app_uart_rx/main.c)
	elseif(${EXAMPLE_TYPE} MATCHES UART_TX)
		add_executable(OBC-firmware.out examples/test_app_uart_tx/main.c)
	elseif(${EXAMPLE_TYPE} MATCHES VN100)
		add_executable(OBC-firmware.out examples/vn100_demo/test_binary_reading/main.c)
	else ()
		message (FATAL_ERROR "Invalid example type: ${EXAMPLE_TYPE} ")
	endif()
else()
	message (FATAL_ERROR "Invalid build type: ${CMAKE_BUILD_TYPE}")
endif()

target_link_options(OBC-firmware.out PRIVATE
    -specs=nosys.specs
    -Wl,-Map=${CMAKE_PROJECT_NAME}.map -T${LINKER_SCRIPT}
    -Wl,--print-memory-usage
)

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
    ENABLE_TASK_STATS_COLLECTOR=${ENABLE_TASK_STATS_COLLECTOR}
)

target_include_directories(OBC-firmware.out PUBLIC
    ${CMAKE_BINARY_DIR}/toolchain/arm-none-eabi/include
)

add_subdirectory(tools/bringup/interface_debug_tool)
add_subdirectory(config)
add_subdirectory(hal)
add_subdirectory(drivers)
add_subdirectory(sys)
add_subdirectory(modules)
add_subdirectory(reliance_edge)

target_link_libraries(OBC-firmware.out PRIVATE
    tiny-aes
    lib-correct
    obc-gs-interface
    ${HAL_LIB_OPTIMIZE}
    $<TARGET_OBJECTS:${HAL_LIB_NO_OPTIMIZE}>
)

# Add OBC-firmware.bin file generation into build process
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
