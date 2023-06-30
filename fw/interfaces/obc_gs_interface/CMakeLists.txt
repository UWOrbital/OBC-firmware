cmake_minimum_required(VERSION 3.15)

set(OBC_GS_INTERFACE_LIB_NAME obc-gs-interface)

set(INCLUDE_DIRS
    commands
    telemetry
)

set(SOURCES
    commands/command_pack.c
    commands/command_unpack.c
    telemetry/telemetry_pack.c
)

add_library(${OBC_GS_INTERFACE_LIB_NAME} STATIC ${SOURCES})
target_include_directories(${OBC_GS_INTERFACE_LIB_NAME} PUBLIC ${INCLUDE_DIRS})
