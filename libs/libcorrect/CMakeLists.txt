cmake_minimum_required(VERSION 3.15)

set(LIBCORRECT_LIB lib-correct)

add_library(${LIBCORRECT_LIB} STATIC
    source/reed-solomon/decode.c
    source/reed-solomon/encode.c
    source/reed-solomon/polynomial.c
    source/reed-solomon/reed-solomon.c
)

target_include_directories(${LIBCORRECT_LIB} PUBLIC
    include
)
