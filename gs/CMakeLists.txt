cmake_minimum_required(VERSION 3.15)

set(SOURCES
    main.c
    common/gs_heap.c
    serial_utils/win_uart.c
)

add_executable(gs ${SOURCES})

target_include_directories(gs PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/common
    ${CMAKE_CURRENT_SOURCE_DIR}/serial_utils
)

target_compile_options(gs PUBLIC -Wall -g)

target_link_libraries(gs PUBLIC
    tiny-aes
    lib-correct
    obc-gs-interface
)
