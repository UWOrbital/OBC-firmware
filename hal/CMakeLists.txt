cmake_minimum_required(VERSION 3.15)

target_include_directories(OBC-firmware.out PUBLIC
    include
)

target_sources(OBC-firmware.out PUBLIC
    source/can.c
    source/dabort.s
    source/errata_SSWF021_45.c
    source/esm.c
    source/gio.c
    source/i2c.c
    source/notification.c
    source/os_croutine.c
    source/os_event_groups.c
    source/os_heap.c
    source/os_list.c
    source/os_mpu_wrappers.c
    source/os_port.c
    source/os_portasm.s
    source/os_queue.c
    source/os_required_fns.c
    source/os_tasks.c
    source/os_timer.c
    source/pinmux.c
    source/sci.c
    source/spi.c
    source/sys_core.s
    source/sys_dma.c
    source/sys_intvecs.s
    source/sys_mpu.s
    source/sys_pcr.c
    source/sys_phantom.c
    source/sys_pmm.c
    source/sys_pmu.s
    source/sys_selftest.c
    source/sys_startup.c
    source/sys_vim.c
    source/system.c
)
