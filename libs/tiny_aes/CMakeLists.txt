cmake_minimum_required(VERSION 3.15)

set(TINY_AES_LIB tiny-aes)

add_library(${TINY_AES_LIB} STATIC
    source/aes.c
)
if(${CMAKE_BUILD_TYPE} MATCHES Test)
target_compile_options(${TINY_AES_LIB} PRIVATE -m32)
endif()
target_include_directories(${TINY_AES_LIB} PUBLIC
    include
)
