cmake_minimum_required(VERSION 3.15)

set(SOURCES
main.c
common/gs_heap.c
common/bindings/c/cserialport.cpp
)

add_executable(gs.out ${SOURCES})

target_include_directories(gs.out PUBLIC
${CMAKE_CURRENT_SOURCE_DIR}/common
${CMAKE_CURRENT_SOURCE_DIR}/common/bindings/c
${CMAKE_CURRENT_SOURCE_DIR}/serial_utils
)

target_compile_options(gs.out PUBLIC -Wall -g)

target_link_libraries(gs.out PUBLIC
tiny-aes
lib-correct
obc-gs-interface
cserialport
)