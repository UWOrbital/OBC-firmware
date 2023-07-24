cmake_minimum_required(VERSION 3.15)

set(TINY_AES_LIB tiny-aes)

add_library(${TINY_AES_LIB} STATIC
    source/aes.c
)

target_include_directories(${TINY_AES_LIB} PUBLIC
    include
)
