cmake_minimum_required(VERSION 3.15)

if (WIN32)
    set(SOURCES
    window.c
    common/gs_heap.c
    serial_utils/win_uart.c
    )
elseif (UNIX)
    set(SOURCES
        wsl.c
        common/gs_heap.c
        serial_utils/wsl_uart.c
    )
endif()

add_executable(gs.out ${SOURCES})

target_include_directories(gs.out PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/common
    ${CMAKE_CURRENT_SOURCE_DIR}/serial_utils
)

target_compile_options(gs.out PUBLIC -Wall -g)

target_link_libraries(gs.out PUBLIC
    tiny-aes
    lib-correct
    obc-gs-interface
)
