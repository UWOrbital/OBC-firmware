set(CMAKE_SYSTEM_NAME Generic)
cmake_minimum_required(VERSION 3.5.0)
project (OBC-firmware C ASM)

set(CMAKE_GENERATOR "Unix Makefiles")
set(CMAKE_C_COMPILER "../compiler/bin/arm-none-eabi-gcc")
set(CMAKE_ASM_COMPILER "../compiler/bin/arm-none-eabi-gcc")

set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_ASM_COMPILER_FORCED TRUE)

set(CMAKE_C_STANDARD 99)

add_compile_options(-mcpu=cortex-r4 -march=armv7-r -mtune=cortex-r4 -marm -mfpu=vfpv3-d16)
add_compile_options(-Os -g -gdwarf-3 -gstrict-dwarf -Wall -Wextra -Wno-unused-parameter -fstack-usage -fdump-ipa-cgraph --specs=nosys.specs  -MMD)

# if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
#     message(STATUS "Maximum optimization for speed")
#     add_compile_options(-Ofast)
# elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
#     message(STATUS "Maximum optimization for speed, debug info included")
#     add_compile_options(-Ofast -g)
# elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
#     message(STATUS "Maximum optimization for size")
#     add_compile_options(-Os)
# else ()
#     message(STATUS "Minimal optimization, debug info included")
#     add_compile_options(-Og -g)
# endif ()

add_definitions(-DDEBUG -DRM46_LAUNCHPAD -DLOG_DEFAULT_OUTPUT_LOCATION=LOG_TO_UART -DLOG_DEFAULT_LEVEL=LOG_TRACE)

# set(CC_FOLDER_ROOT C:/Program Files (x86)/Arm GNU Toolchain arm-none-eabi/11.2 2022.02)

include_directories(
    compiler/arm-none-eabi/include
    hal/include
    drivers/include
    common/include
    adcs/include
    cdh/include
    comms/include
    eps/include
    payload/include
    )

set(SOURCES
    main.c
    adcs/source/adcs_manager.c
    cdh/source/supervisor.c
    cdh/source/telemetry.c
    common/source/obc_logging.c
    comms/source/comms_manager.c
    drivers/source/obc_i2c_io.c
    drivers/source/obc_sci_io.c
    drivers/source/obc_spi_io.c
    eps/source/eps_manager.c
    hal/source/dabort.s
    hal/source/errata_SSWF021_45.c
    hal/source/esm.c
    hal/source/gio.c
    hal/source/i2c.c
    hal/source/notification.c
    hal/source/os_croutine.c
    hal/source/os_event_groups.c
    hal/source/os_heap.c
    hal/source/os_list.c
    hal/source/os_mpu_wrappers.c
    hal/source/os_port.c
    hal/source/os_portasm.s
    hal/source/os_queue.c
    hal/source/os_required_fns.c
    hal/source/os_tasks.c
    hal/source/os_timer.c
    hal/source/pinmux.c
    hal/source/sci.c
    hal/source/spi.c
    hal/source/sys_calls.c
    hal/source/sys_core.s
    hal/source/sys_dma.c
    hal/source/sys_intvecs.s
    hal/source/sys_mpu.s
    hal/source/sys_pcr.c
    hal/source/sys_phantom.c
    hal/source/sys_pmm.c
    hal/source/sys_pmu.s
    hal/source/sys_selftest.c
    hal/source/sys_startup.c
    hal/source/sys_vim.c
    hal/source/system.c
    payload/source/payload_manager.c
    )

set(LINKER_SCRIPT ../hal/source/sys_link.ld)

# add_link_options(-Wl,-gc-sections,--print-memory-usage,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map)
add_link_options(-Wl,-Map,${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map)
add_link_options(-T ${LINKER_SCRIPT})

add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})

set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)
