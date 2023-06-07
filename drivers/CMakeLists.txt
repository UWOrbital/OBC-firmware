cmake_minimum_required(VERSION 3.15)

target_include_directories(OBC-firmware.out PUBLIC
    cc1120/include
    common/include
    ds3232/include
    fram/include
    lm75bd/include
)

target_sources(OBC-firmware.out PUBLIC
    cc1120/source/cc1120_mcu.c
    cc1120/source/cc1120_spi.c
    cc1120/source/cc1120_txrx.c
    common/source/obc_i2c_io.c
    common/source/obc_reset.c
    common/source/obc_spi_io.c
    common/source/obc_sci_io.c
    common/source/obc_sw_watchdog.c
    ds3232/source/ds3232_mz.c
    fram/source/fram.c
    lm75bd/source/lm75bd.c
)
