cmake_minimum_required(VERSION 3.15)

set(CSerialPortRootPath "/home/joel/CSerialPort/bin/install")

set(SOURCES
main.c
common/gs_heap.c
)

add_executable(gs.out ${SOURCES})

target_include_directories(gs.out PUBLIC
${CMAKE_CURRENT_SOURCE_DIR}/common
${CMAKE_CURRENT_SOURCE_DIR}/serial_utils
${CSerialPortRootPath}/include
)

target_link_directories(gs.out PUBLIC ${CSerialPortRootPath}/lib)

target_compile_options(gs.out PUBLIC -Wall -g)

target_link_libraries(gs.out PUBLIC
tiny-aes
lib-correct
obc-gs-interface
if (WIN32)
   libcserialport
elseif(UNIX)
    cserialport
endif()
)


