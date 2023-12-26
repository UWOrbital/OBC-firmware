cmake_minimum_required (VERSION 3.20)

project(OBCExamples)

# path to current source dir
set (path ${CMAKE_CURRENT_SOURCE_DIR})

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
${parentPath}/drivers/rm46/obc_gio_ctrl.c
${parentPath}/drivers/rm46/obc_reset.c

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
${parentPath}/hal/launchpad/source/os_mpu_wrappers.c
${parentPath}/hal/launchpad/source/i2c.c
${grandparentPath}/libs/posix_freertos/source/os_tasks.c
${grandparentPath}/libs/posix_freertos/source/os_queue.c
${grandparentPath}/libs/posix_freertos/source/os_required_fns.c
${grandparentPath}/libs/posix_freertos/source/os_list.c
#${grandparentPath}/libs/posix_freertos/source/os_timers.c


)

set (INCLUDE_DIRS
${grandparentPath}/libs/posix_freertos/include
${parentPath}/sys
${parentPath}/sys/print
${parentPath}/sys/logging
${parentPath}/sys/utils
${parentPath}/sys/time
${parentPath}/build_arm/obc
${parentPath}/config
${parentPath}/modules/comms_link_mgr
${parentPath}/modules/alarm_mgr
${parentPath}/modules/camera_mgr
${parentPath}/modules/digital_watchdog_mgr
${parentPath}/modules/command_mgr
${parentPath}/modules/eps_mgr
${parentPath}/modules/state_mgr
${parentPath}/modules/logger
${parentPath}/hal/common
${parentPath}/hal/launchpad/include
${parentPath}/hal/launchpad/source
${parentPath}/drivers/rm46
${parentPath}/drivers/ds3232
${parentPath}/drivers/fram
${parentPath}/drivers/lm75bd
${parentPath}/drivers/cc1120
${parentPath}/drivers/sdcard
${parentPath}/drivers/tpl5010
${parentPath}/reliance_edge/bdev
${parentPath}/reliance_edge/fse
${parentPath}/reliance_edge/posix
${parentPath}/reliance_edge/core/include
${parentPath}/reliance_edge/include
${parentPath}/reliance_edge/util
${parentPath}/reliance_edge/os/freertos/services
${parentPath}/reliance_edge/os/freertos/include
${parentPath}/reliance_edge/projects/freertos_rm46/host
${grandparentPath}/interfaces/obc_gs_interface/commands

)

elseif (${CMAKE_BUILD_TYPE} MATCHES FRAM_PERSIST)

elseif (${CMAKE_BUILD_TYPE} MATCHES FRAM_SPI)

elseif (${CMAKE_BUILD_TYPE} MATCHES FREERTOS_POSIX)

elseif (${CMAKE_BUILD_TYPE} MATCHES LM75BD)

else()
	message (FATAL_ERROR "Invalid build option: ${CMAKE_BUILD_TYPE}")
endif()

add_executable (OBC-example.out main.c)

target_include_directories(OBC-example.out PUBLIC ${INCLUDE_DIRS})

target_sources(OBC-example.out PUBLIC ${SOURCES})


target_compile_options(OBC-example.out PRIVATE -c -mcpu=cortex-r4 -march=armv7-r -mtune=cortex-r4 -marm -mfpu=vfpv3-d16 ${BOARD_TYPE} -DprojCOVERAGE_TEST=\"1\" -O0 -g -gdwarf-3 -gstrict-dwarf -MMD)
