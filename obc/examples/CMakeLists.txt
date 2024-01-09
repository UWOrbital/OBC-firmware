cmake_minimum_required (VERSION 3.21)

project(OBCExamples LANGUAGES C)

# path to current source dir
set (path ${CMAKE_CURRENT_SOURCE_DIR})

set (EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build_example)

cmake_path(GET path PARENT_PATH parentPath)
cmake_path (GET parentPath PARENT_PATH grandparentPath)

#message("Current path is \"${path}\"")
#message("GrandParent path is \"${grandparentPath}\"")

set (BOARD_TYPE "-DRM46_LAUNCHPAD")
set (CMAKE_EXE_LINKER_FLAGS "--specs=nosys.specs")
set (CMAKE_C_COMPILER "${grandparentPath}/build_arm/toolchain/bin/arm-none-eabi-gcc")



if (${CMAKE_BUILD_TYPE} MATCHES DMA_SPI)

set (HEADERS
${parentPath}/sys/logging/obc_logging.h
${parentPath}/drivers/rm46/obc_sci_io.h
${parentPath}/sys/print/obc_print.h
${parentPath}/sys/obc_errors.h
${parentPath}/drivers/rm46/obc_i2c_io.h
${parentPath}/drivers/rm46/obc_spi_io.h
${parentPath}/hal/launchpad/include/sys_dma.h
${parentPath}/hal/launchpad/include/mibspi.h
${parentPath}/hal/launchpad/include/sys_dma.h
${parentPath}/hal/launchpad/include/gio.h
${parentPath}/hal/launchpad/include/sci.h
${parentPath}/hal/launchpad/include/i2c.h
${parentPath}/hal/launchpad/include/spi.h
${parentPath}/hal/launchpad/include/can.h
${parentPath}/hal/launchpad/include/het.h
${parentPath}/hal/launchpad/include/system.h
${parentPath}/hal/launchpad/source/os_event_groups.c
${parentPath}/hal/launchpad/source/sci.c
${parentPath}/hal/launchpad/source/spi.c
${parentPath}/hal/launchpad/source/sys_dma.c
${parentPath}/hal/launchpad/source/os_mpu_wrappers.c
${parentPath}/hal/launchpad/source/sys_core.s
${parentPath}/hal/launchpad/source/os_queue.c
${parentPath}/hal/launchpad/source/gio.c
${parentPath}/hal/launchpad/source/os_tasks.c
${parentPath}/hal/launchpad/source/os_list.c
${parentPath}/hal/launchpad/source/os_croutine.c
${parentPath}/hal/launchpad/source/os_port.c
${parentPath}/hal/launchpad/source/os_portasm.s
${parentPath}/hal/common/os_required_fns.c
${parentPath}/reliance_edge/posix/posix.c
${parentPath}/drivers/rm46/obc_spi_dma.c
${parentPath}/drivers/rm46/obc_spi_io.c
${parentPath}/drivers/rm46/obc_sci_io.c
${parentPath}/drivers/rm46/obc_dma.c
${parentPath}/drivers/rm46/obc_gio_ctrl.c
${parentPath}/modules/comms_link_mgr/cc1120_txrx.c
${parentPath}/modules/alarm_mgr/alarm_handler.c
${parentPath}/sys/print/obc_print.c
${parentPath}/modules/logger/logger.c
${parentPath}/reliance_edge/os/freertos/services/ostask.c
${parentPath}/reliance_edge/os/freertos/services/osmutex.c
)

set (INCLUDE_DIRS
${parentPath}/modules/logger
${parentPath}/modules/comms_link_mgr
${parentPath}/modules/alarm_mgr
${parentPath}/modules/command_mgr
${parentPath}/sys/print
${parentPath}/sys/logging
${parentPath}/sys/utils
${parentPath}/sys/time
${parentPath}/sys
${parentPath}/drivers/ds3232
${parentPath}/drivers/rm46
${parentPath}/drivers/tpl5010
${parentPath}/drivers/cc1120
${parentPath}/hal/launchpad
${parentPath}/hal/launchpad/include
${parentPath}/hal/launchpad/source
${parentPath}/config
${parentPath}/reliance_edge/include
${parentPath}/reliance_edge/projects/freertos_rm46/host
${parentPath}/reliance_edge/os/freertos/include
${grandparentPath}/interfaces/obc_gs_interface/commands
)
endif()



#message ("Current source dir is \"${CMAKE_CURRENT_SOURCE_DIR}\"")

get_filename_component(INCLUDE_DIRS_ABS "${INCLUDE_DIRS}" ABSOLUTE)
get_filename_component(HEADERS_ABS "${HEADERS}" ABSOLUTE)
#message ("Absolute path is \"${INCLUDE_DIRS_ABS}\"")


#message("Current HEADERS are \"${HEADERS}\"")


set (EXAMPLE_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/main.c)

add_executable (OBC-example.out ${EXAMPLE_SOURCE} ${HEADERS_ABS})

add_library(example-lib STATIC ${HEADERS_ABS})

target_compile_definitions (example-lib PRIVATE RM46_LAUNCHPAD)

target_include_directories(example-lib PUBLIC ${INCLUDE_DIRS_ABS})
set_target_properties (example-lib PROPERTIES LINKER_LANGUAGE C)

target_include_directories(OBC-example.out PUBLIC ${INCLUDE_DIRS_ABS})
target_link_libraries(OBC-example.out example-lib)
#target_include_directories (OBC-example.out PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


target_compile_options(example-lib PUBLIC -c -mcpu=cortex-r4 -march=armv7-r -mtune=cortex-r4 -marm -mfpu=vfpv3-d16 ${BOARD_TYPE} -O0 -g -gdwarf-3 -gstrict-dwarf)
target_compile_options(OBC-example.out PUBLIC -c -mcpu=cortex-r4 -march=armv7-r -mtune=cortex-r4 -marm -mfpu=vfpv3-d16 ${BOARD_TYPE} -O0 -g -gdwarf-3 -gstrict-dwarf)
