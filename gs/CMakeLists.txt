cmake_minimum_required(VERSION 3.15)

set(CSerialPortRootPath "/home/$ENV{USER}/CSerialPort/bin/install")

set(SOURCES
main.c
common/gs_heap.c
common/cserialport.cpp
)

add_executable(gs.out ${SOURCES})

target_include_directories(gs.out PUBLIC
${CMAKE_CURRENT_SOURCE_DIR}/common
${CMAKE_CURRENT_SOURCE_DIR}/serial_utils
${CSerialPortRootPath}/include
)

target_link_directories(gs.out PUBLIC ${CSerialPortRootPath}/lib)

target_compile_options(gs.out PUBLIC -Wall -g)

if (WIN32)
    target_link_libraries(gs.out PUBLIC
    tiny-aes
    lib-correct
    obc-gs-interface
    libcserialport
    )
elseif(UNIX)
    target_link_libraries(gs.out PUBLIC
    tiny-aes
    lib-correct
    obc-gs-interface
    cserialport
    )
endif()
