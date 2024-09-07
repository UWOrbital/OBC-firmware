cmake_minimum_required(VERSION 3.15)

set(SOURCES
    main.c
)

add_executable(gs.out ${SOURCES})

target_include_directories(gs.out PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/common
    ${CMAKE_CURRENT_SOURCE_DIR}/serial_utils
)

target_compile_options(gs.out PUBLIC -Wall -g)

if(UNIX)
    target_link_libraries(gs.out PUBLIC
        tiny-aes
        rs-lib
        obc-gs-interface
        CSerialPort
        Threads::Threads
)
elseif(WIN32)
    target_link_libraries(gs.out PUBLIC
        tiny-aes
        rs-lib
        obc-gs-interface
        CSerialPort
        setupapi
        Threads::Threads
)
endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
