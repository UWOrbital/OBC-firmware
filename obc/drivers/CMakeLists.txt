cmake_minimum_required(VERSION 3.15)

target_include_directories(OBC-firmware.out PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/rm46
    ${CMAKE_CURRENT_SOURCE_DIR}/arducam
    ${CMAKE_CURRENT_SOURCE_DIR}/cc1120
    ${CMAKE_CURRENT_SOURCE_DIR}/ds3232
    ${CMAKE_CURRENT_SOURCE_DIR}/fram
    ${CMAKE_CURRENT_SOURCE_DIR}/lm75bd
    ${CMAKE_CURRENT_SOURCE_DIR}/max5360
    ${CMAKE_CURRENT_SOURCE_DIR}/rffm6404
    ${CMAKE_CURRENT_SOURCE_DIR}/sdcard
    ${CMAKE_CURRENT_SOURCE_DIR}/sw_watchdog
    ${CMAKE_CURRENT_SOURCE_DIR}/tpl5010
    ${CMAKE_CURRENT_SOURCE_DIR}/bd621x
    ${CMAKE_CURRENT_SOURCE_DIR}/vn100
    # TODO: Update cc1120 driver so that it doesn't need to include this
    ${CMAKE_CURRENT_SOURCE_DIR}../services/comms_link_manager
)

target_sources(OBC-firmware.out PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/rm46/obc_gio_ctrl.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rm46/obc_het_ctrl.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rm46/obc_i2c_io.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rm46/obc_reset.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rm46/obc_sci_io.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rm46/obc_spi_io.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rm46/obc_spi_dma.c

    ${CMAKE_CURRENT_SOURCE_DIR}/arducam/arducam.c
    ${CMAKE_CURRENT_SOURCE_DIR}/arducam/camera_reg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/arducam/ov5642_reg.c

    ${CMAKE_CURRENT_SOURCE_DIR}/cc1120/cc1120_mcu.c
    ${CMAKE_CURRENT_SOURCE_DIR}/cc1120/cc1120_spi.c
    ${CMAKE_CURRENT_SOURCE_DIR}/cc1120/cc1120_txrx.c

    ${CMAKE_CURRENT_SOURCE_DIR}/ds3232/ds3232_mz.c

    ${CMAKE_CURRENT_SOURCE_DIR}/fram/fm25v20a.c

    ${CMAKE_CURRENT_SOURCE_DIR}/lm75bd/lm75bd.c

    ${CMAKE_CURRENT_SOURCE_DIR}/max5360/max5360.c

    ${CMAKE_CURRENT_SOURCE_DIR}/rffm6404/rffm6404.c

    ${CMAKE_CURRENT_SOURCE_DIR}/sdcard/sdc_bdev.c

    ${CMAKE_CURRENT_SOURCE_DIR}/bd621x/bd621x.c

    ${CMAKE_CURRENT_SOURCE_DIR}/sw_watchdog/obc_sw_watchdog.c

    ${CMAKE_CURRENT_SOURCE_DIR}/tpl5010/tpl5010.c
    ${CMAKE_CURRENT_SOURCE_DIR}/vn100/vn100_ascii.c
    ${CMAKE_CURRENT_SOURCE_DIR}/vn100/vn100_binary.c
    ${CMAKE_CURRENT_SOURCE_DIR}/vn100/vn100_common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/vn100/vn100_parsing.c
)
