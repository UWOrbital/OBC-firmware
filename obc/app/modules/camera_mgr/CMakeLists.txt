cmake_minimum_required(VERSION 3.15)

target_include_directories(${OUT_FILE_NAME} PUBLIC
    include
)

target_sources(${OUT_FILE_NAME} PUBLIC
    source/arducam.c
    source/camera_reg.c
    source/ov5642_reg.c
    source/payload_manager.c
)
