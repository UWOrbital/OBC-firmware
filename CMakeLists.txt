cmake_minimum_required(VERSION 3.15)

include_directories(
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
    payload/source/payload_manager.c
)

if(CMAKE_BUILD_TYPE MATCHES Firmware)
    file (STRINGS ".env" TOOLCHAIN_PREFIX)

    set(CMAKE_TOOLCHAIN_FILE cmake/toolchain_arm_none_eabi.cmake)
    set(LINKER_SCRIPT ../hal/source/sys_link.ld)
    set(PROJECT_EXTENSION out)

    include_directories(
        ${TOOLCHAIN_PREFIX}/arm-none-eabi/include
        hal/include
    )

    set(SOURCES
        ${SOURCES}
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
    )
elseif(CMAKE_BUILD_TYPE MATCHES Test)
    set(PROJECT_EXTENSION exe)
else()
    message(FATAL_ERROR "Invalid build type: ${CMAKE_BUILD_TYPE}")
endif()

project(OBC-firmware LANGUAGES C ASM)

add_executable(${PROJECT_NAME}.${PROJECT_EXTENSION} ${SOURCES})
