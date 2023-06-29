cmake_minimum_required(VERSION 3.15)

set(RM46_LAUNCHPAD_HAL_LIB rm46-launchpad-hal)
set(OBC_REV1_HAL_LIB obc-rev1-hal)
set(OBC_REV2_HAL_LIB obc-rev2-hal)

add_library(${RM46_LAUNCHPAD_HAL_LIB} STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/common/os_required_fns.c
    ${CMAKE_CURRENT_SOURCE_DIR}/launchpad/source/can.c
    ${CMAKE_CURRENT_SOURCE_DIR}/launchpad/source/dabort.s
    ${CMAKE_CURRENT_SOURCE_DIR}/launchpad/source/errata_SSWF021_45.c
    ${CMAKE_CURRENT_SOURCE_DIR}/launchpad/source/esm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/launchpad/source/gio.c
    ${CMAKE_CURRENT_SOURCE_DIR}/launchpad/source/het.c
    ${CMAKE_CURRENT_SOURCE_DIR}/launchpad/source/i2c.c
    ${CMAKE_CURRENT_SOURCE_DIR}/launchpad/source/notification.c
    ${CMAKE_CURRENT_SOURCE_DIR}/launchpad/source/os_croutine.c
    ${CMAKE_CURRENT_SOURCE_DIR}/launchpad/source/os_event_groups.c
    ${CMAKE_CURRENT_SOURCE_DIR}/launchpad/source/os_heap.c
    ${CMAKE_CURRENT_SOURCE_DIR}/launchpad/source/os_list.c
    ${CMAKE_CURRENT_SOURCE_DIR}/launchpad/source/os_mpu_wrappers.c
    ${CMAKE_CURRENT_SOURCE_DIR}/launchpad/source/os_port.c
    ${CMAKE_CURRENT_SOURCE_DIR}/launchpad/source/os_portasm.s
    ${CMAKE_CURRENT_SOURCE_DIR}/launchpad/source/os_queue.c
    ${CMAKE_CURRENT_SOURCE_DIR}/launchpad/source/os_tasks.c
    ${CMAKE_CURRENT_SOURCE_DIR}/launchpad/source/os_timer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/launchpad/source/pinmux.c
    ${CMAKE_CURRENT_SOURCE_DIR}/launchpad/source/sci.c
    ${CMAKE_CURRENT_SOURCE_DIR}/launchpad/source/spi.c
    ${CMAKE_CURRENT_SOURCE_DIR}/launchpad/source/sys_core.s
    ${CMAKE_CURRENT_SOURCE_DIR}/launchpad/source/sys_dma.c
    ${CMAKE_CURRENT_SOURCE_DIR}/launchpad/source/sys_intvecs.s
    ${CMAKE_CURRENT_SOURCE_DIR}/launchpad/source/sys_mpu.s
    ${CMAKE_CURRENT_SOURCE_DIR}/launchpad/source/sys_pcr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/launchpad/source/sys_phantom.c
    ${CMAKE_CURRENT_SOURCE_DIR}/launchpad/source/sys_pmm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/launchpad/source/sys_pmu.s
    ${CMAKE_CURRENT_SOURCE_DIR}/launchpad/source/sys_selftest.c
    ${CMAKE_CURRENT_SOURCE_DIR}/launchpad/source/sys_startup.c
    ${CMAKE_CURRENT_SOURCE_DIR}/launchpad/source/sys_vim.c
    ${CMAKE_CURRENT_SOURCE_DIR}/launchpad/source/system.c
)

target_include_directories(${RM46_LAUNCHPAD_HAL_LIB} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/launchpad/include
    ${CMAKE_CURRENT_SOURCE_DIR}/common
)

add_library(${OBC_REV1_HAL_LIB} STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/common/os_required_fns.c
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev1/source/can.c
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev1/source/dabort.s
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev1/source/errata_SSWF021_45.c
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev1/source/esm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev1/source/gio.c
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev1/source/het.c
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev1/source/i2c.c
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev1/source/notification.c
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev1/source/os_croutine.c
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev1/source/os_event_groups.c
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev1/source/os_heap.c
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev1/source/os_list.c
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev1/source/os_mpu_wrappers.c
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev1/source/os_port.c
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev1/source/os_portasm.s
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev1/source/os_queue.c
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev1/source/os_tasks.c
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev1/source/os_timer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev1/source/pinmux.c
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev1/source/sci.c
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev1/source/spi.c
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev1/source/sys_core.s
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev1/source/sys_dma.c
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev1/source/sys_intvecs.s
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev1/source/sys_mpu.s
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev1/source/sys_pcr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev1/source/sys_phantom.c
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev1/source/sys_pmm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev1/source/sys_pmu.s
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev1/source/sys_selftest.c
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev1/source/sys_startup.c
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev1/source/sys_vim.c
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev1/source/system.c
)

target_include_directories(${OBC_REV1_HAL_LIB} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev1/include
    ${CMAKE_CURRENT_SOURCE_DIR}/common
)

add_library(${OBC_REV2_HAL_LIB} STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/common/os_required_fns.c
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev2/source/can.c
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev2/source/dabort.s
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev2/source/errata_SSWF021_45.c
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev2/source/esm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev2/source/gio.c
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev2/source/het.c
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev2/source/i2c.c
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev2/source/notification.c
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev2/source/os_croutine.c
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev2/source/os_event_groups.c
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev2/source/os_heap.c
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev2/source/os_list.c
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev2/source/os_mpu_wrappers.c
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev2/source/os_port.c
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev2/source/os_portasm.s
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev2/source/os_queue.c
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev2/source/os_tasks.c
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev2/source/os_timer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev2/source/pinmux.c
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev2/source/sci.c
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev2/source/spi.c
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev2/source/sys_core.s
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev2/source/sys_dma.c
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev2/source/sys_intvecs.s
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev2/source/sys_mpu.s
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev2/source/sys_pcr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev2/source/sys_phantom.c
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev2/source/sys_pmm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev2/source/sys_pmu.s
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev2/source/sys_selftest.c
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev2/source/sys_startup.c
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev2/source/sys_vim.c
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev2/source/system.c
)

target_include_directories(${OBC_REV2_HAL_LIB} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_rev2/include
    ${CMAKE_CURRENT_SOURCE_DIR}/common
)
