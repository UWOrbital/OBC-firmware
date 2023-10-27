cmake_minimum_required(VERSION 3.15)

SET(INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}/rm46
)

SET(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/rm46/obc_gio_ctrl.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rm46/obc_het_ctrl.c
)

target_include_directories(OBC-firmware.out PUBLIC ${INCLUDES})
target_sources(OBC-firmware.out PUBLIC ${SOURCES})
