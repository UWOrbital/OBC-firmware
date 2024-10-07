cmake_minimum_required(VERSION 3.15)

target_include_directories(${OUT_FILE_NAME} PUBLIC
    include
)

target_sources(${OUT_FILE_NAME} PUBLIC
    source/eps_manager.c
)
