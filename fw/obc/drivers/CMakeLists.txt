cmake_minimum_required(VERSION 3.15)

target_include_directories(OBC-firmware.out PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_wrappers
    ${CMAKE_CURRENT_SOURCE_DIR}/peripherals/arducam
    ${CMAKE_CURRENT_SOURCE_DIR}/peripherals/cc1120
    ${CMAKE_CURRENT_SOURCE_DIR}/peripherals/ds3232
    ${CMAKE_CURRENT_SOURCE_DIR}/peripherals/fram
    ${CMAKE_CURRENT_SOURCE_DIR}/peripherals/lm75bd
    ${CMAKE_CURRENT_SOURCE_DIR}/peripherals/sdcard
    ${CMAKE_CURRENT_SOURCE_DIR}/peripherals/sw_wdt
)

target_sources(OBC-firmware.out PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_wrappers/obc_gio_ctrl.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_wrappers/obc_het_ctrl.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_wrappers/obc_i2c_io.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_wrappers/obc_reset.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_wrappers/obc_sci_io.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_wrappers/obc_spi_io.c

    ${CMAKE_CURRENT_SOURCE_DIR}/peripherals/arducam/arducam.c
    ${CMAKE_CURRENT_SOURCE_DIR}/peripherals/arducam/camera_reg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/peripherals/arducam/ov5642_reg.c

    ${CMAKE_CURRENT_SOURCE_DIR}/peripherals/cc1120/cc1120_mcu.c
    ${CMAKE_CURRENT_SOURCE_DIR}/peripherals/cc1120/cc1120_spi.c
    ${CMAKE_CURRENT_SOURCE_DIR}/peripherals/cc1120/cc1120_txrx.c

    ${CMAKE_CURRENT_SOURCE_DIR}/peripherals/ds3232/ds3232_mz.c

    ${CMAKE_CURRENT_SOURCE_DIR}/peripherals/fram/fram.c

    ${CMAKE_CURRENT_SOURCE_DIR}/peripherals/lm75bd/lm75bd.c

    ${CMAKE_CURRENT_SOURCE_DIR}/peripherals/sdcard/sdc_bdev.c

    ${CMAKE_CURRENT_SOURCE_DIR}/peripherals/sw_wdt/obc_sw_watchdog.c
)
