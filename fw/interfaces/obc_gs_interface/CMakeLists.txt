cmake_minimum_required(VERSION 3.15)

set(OBC_GS_INTERFACE_LIB_NAME obc-gs-interface)

set(INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/common
    ${CMAKE_CURRENT_SOURCE_DIR}/commands
    ${CMAKE_CURRENT_SOURCE_DIR}/telemetry
    ${CMAKE_CURRENT_SOURCE_DIR}/aes128
    ${CMAKE_CURRENT_SOURCE_DIR}/ax25
    ${CMAKE_CURRENT_SOURCE_DIR}/fec

    ${CMAKE_CURRENT_SOURCE_DIR}/../common
)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/obc_pack_utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/obc_unpack_utils.c

    ${CMAKE_CURRENT_SOURCE_DIR}/commands/command_pack.c
    ${CMAKE_CURRENT_SOURCE_DIR}/commands/command_unpack.c
    ${CMAKE_CURRENT_SOURCE_DIR}/telemetry/telemetry_pack.c

    ${CMAKE_CURRENT_SOURCE_DIR}/aes128/obc_gs_aes128.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ax25/obc_gs_ax25.c
    ${CMAKE_CURRENT_SOURCE_DIR}/fec/obc_gs_fec.c
)

add_library(${OBC_GS_INTERFACE_LIB_NAME} STATIC ${SOURCES})
target_include_directories(${OBC_GS_INTERFACE_LIB_NAME} PUBLIC ${INCLUDE_DIRS})

target_link_libraries(${OBC_GS_INTERFACE_LIB_NAME} PRIVATE
    tiny-aes
    lib-correct
)
