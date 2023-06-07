cmake_minimum_required(VERSION 3.15)

target_include_directories(OBC-firmware.out PUBLIC
    include
)

target_sources(OBC-firmware.out PUBLIC
    source/reed-solomon/decode.c
    source/reed-solomon/encode.c
    source/reed-solomon/polynomial.c
    source/reed-solomon/reed-solomon.c
)
