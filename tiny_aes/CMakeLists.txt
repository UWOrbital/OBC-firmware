cmake_minimum_required(VERSION 3.15)

target_include_directories(OBC-firmware.out PUBLIC
    include
)

target_sources(OBC-firmware.out PUBLIC
    source/aes.c
)
