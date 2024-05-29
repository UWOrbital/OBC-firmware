cmake_minimum_required(VERSION 3.15)

set(SOURCES
    hil_main.cpp
)

add_executable(hil.out ${SOURCES})

target_include_directories(hil.out PUBLIC
)

target_compile_options(hil.out PUBLIC -Wall -g)

#target_link_libraries(hil.out PUBLIC)
        