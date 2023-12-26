cmake_minimum_required (VERSION 3.20)

project(OBCExamples)

# path to current source dir
set (path ${CMAKE_CURRENT_SOURCE_DIR})

set (EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build_example)

cmake_path(GET path PARENT_PATH parentPath)
cmake_path (GET parentPath PARENT_PATH grandparentPath)

message("Current path is \"${path}\"")
message("GrandParent path is \"${grandparentPath}\"")

set (BOARD_TYPE "-DRM46_LAUNCHPAD")
set (CMAKE_EXE_LINKER_FLAGS "--specs=nosys.specs")
set (CMAKE_C_COMPILER "${grandparentPath}/build_arm/toolchain/bin/arm-none-eabi-gcc")



if (${CMAKE_BUILD_TYPE} MATCHES DMA_SPI)
set (SOURCES
${parentPath}/sys/print/obc_print.c
${parentPath}/drivers/rm46/obc_sci_io.c
${parentPath}/drivers/rm46/obc_digital_watchdog.c
${parentPath}/drivers/rm46/obc_dma.c
${parentPath}/drivers/rm46/obc_gio_ctrl.c
${parentPath}/drivers/rm46/obc_het_ctrl.c
${parentPath}/drivers/rm46/obc_i2c_io.c
${parentPath}/drivers/rm46/obc_reset.c
${parentPath}/drivers/rm46/obc_sci_io.c
${parentPath}/drivers/rm46/obc_spi_io.c
${parentPath}/drivers/rm46/obc_spi_dma.c
${parentPath}/drivers/ds3232/ds3232_mz.c
${parentPath}/drivers/fram/fm25v20a.c
${parentPath}/drivers/lm75bd/lm75bd.c
${parentPath}/drivers/cc1120/cc1120.c
${parentPath}/modules/logger/logger.c
${parentPath}/modules/comms_link_mgr/cc1120_txrx.c
${parentPath}/reliance_edge/bdev/bdev.c
${parentPath}/reliance_edge/fse/fse.c
${parentPath}/reliance_edge/posix/posix.c
${parentPath}/reliance_edge/posix/path.c
${parentPath}/reliance_edge/projects/freertos_rm46/host/redconf.c
${parentPath}/reliance_edge/os/freertos/services/osassert.c
${parentPath}/reliance_edge/os/freertos/services/osbdev.c
${parentPath}/reliance_edge/os/freertos/services/osclock.c
${parentPath}/reliance_edge/os/freertos/services/osmutex.c
${parentPath}/reliance_edge/os/freertos/services/osoutput.c
${parentPath}/reliance_edge/os/freertos/services/ostimestamp.c
${parentPath}/reliance_edge/os/freertos/services/osuidgid.c
${parentPath}/reliance_edge/core/driver/core.c
${parentPath}/reliance_edge/util/bitmap.c
${parentPath}/reliance_edge/util/crc.c
${parentPath}/reliance_edge/util/endian.c
${parentPath}/reliance_edge/util/ftype.c
${parentPath}/reliance_edge/util/heap.c
${parentPath}/reliance_edge/util/memory.c
${parentPath}/reliance_edge/util/namelen.c
${parentPath}/reliance_edge/util/perm.c
${parentPath}/reliance_edge/util/sign.c
${parentPath}/reliance_edge/util/string.c
${parentPath}/hal/launchpad/source/spi.c
${parentPath}/hal/launchpad/source/sci.c
${parentPath}/hal/launchpad/source/sys_dma.c
${parentPath}/hal/launchpad/source/sys_core.s
#${parentPath}/hal/launchpad/source/os_mpu_wrappers.c
${parentPath}/hal/launchpad/source/i2c.c
${parentPath}/hal/launchpad/source/gio.c
${grandparentPath}/libs/posix_freertos/source/os_tasks.c
${grandparentPath}/libs/posix_freertos/source/os_queue.c
${grandparentPath}/libs/posix_freertos/source/os_required_fns.c
${grandparentPath}/libs/posix_freertos/source/os_list.c
${grandparentPath}/libs/posix_freertos/source/os_port.c
#${grandparentPath}/libs/posix_freertos/source/os_timers.c


)

set (HEADERS
${parentPath}/sys/logging/obc_logging.h
${parentPath}/drivers/rm46/obc_sci_io.h
${parentPath}/sys/print/obc_print.h
${parentPath}/drivers/rm46/obc_i2c_io.h
${parentPath}/drivers/rm46/obc_spi_io.h
${parentPath}/hal/launchpad/include/sys_dma.h
${parentPath}/hal/launchpad/include/mibspi.h
${parentPath}/drivers/rm46/obc_spi_dma.h
${parentPath}/drivers/rm46/obc_privilege.h
${parentPath}/hal/launchpad/include/FreeRTOS.h
${parentPath}/hal/launchpad/include/os_task.h
${parentPath}/hal/launchpad/include/sys_common.h
${parentPath}/hal/launchpad/include/sys_core.h
${parentPath}/hal/launchpad/include/gio.h
${parentPath}/hal/launchpad/include/sci.h
${parentPath}/hal/launchpad/include/i2c.h
${parentPath}/hal/launchpad/include/spi.h
${parentPath}/hal/launchpad/include/can.h
${parentPath}/hal/launchpad/include/het.h
${parentPath}/hal/launchpad/include/system.h
)

set (INCLUDE_DIRS
${parentPath}/sys/print
${parentPath}/sys/logging
${parentPath}/sys
${parentPath}/drivers/rm46
${parentPath}/hal/launchpad/include
)

endif()

message("Current HEADERS are \"${HEADERS}\"")


set (EXAMPLE_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/dma_spi_demo/main.c)

add_executable (OBC-example.out ${EXAMPLE_SOURCE} ${HEADERS})

target_link_libraries(OBC-example.out ${HEADERS})
#target_include_directories(OBC-example.out PRIVATE ${INCLUDE_DIRS})

#target_sources(OBC-example.out PUBLIC ${SOURCES})


#target_compile_options(OBC-example.out PRIVATE -c -mcpu=cortex-r4 -march=armv7-r -mtune=cortex-r4 -marm -mfpu=vfpv3-d16 ${BOARD_TYPE} -O0 -g -gdwarf-3 -gstrict-dwarf)
