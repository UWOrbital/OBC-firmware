cmake_minimum_required(VERSION 3.15)

target_include_directories(OBC-firmware.out PUBLIC
    include
)

target_sources(OBC-firmware.out PUBLIC
    source/obc_i2c_io.c
    source/obc_sci_io.c
    source/obc_spi_io.c
    source/ds3232_mz.c
    source/lm75bd.c
)