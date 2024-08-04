cmake_minimum_required(VERSION 3.15)

set(MBED_LIB mbed)

add_library(${MBED_LIB} STATIC
    source/aes.c
    source/cipher.c
    source/gcm.c
    source/ccm.c
    source/cipher_wrap.c
)

target_include_directories(${MBED_LIB} PUBLIC
    include
)

target_compile_definitions(${MBED_LIB} PRIVATE
    MBEDTLS_CONFIG_FILE="mbedtls_config.h"
)
