cmake_minimum_required(VERSION 3.15)

if(${CMAKE_BUILD_TYPE} MATCHES GS)
    project(Orbital-gs LANGUAGES C CXX)
    add_subdirectory(gs)
endif()

if(${CMAKE_BUILD_TYPE} MATCHES OBC)
    set(CMAKE_TOOLCHAIN_FILE "obc/cmake/toolchain_arm_none_eabi.cmake")

    project(Orbital-fw LANGUAGES C ASM)

    add_subdirectory(obc)
endif()

if(${CMAKE_BUILD_TYPE} MATCHES Test)
    project(Orbital-test LANGUAGES C CXX)
    add_subdirectory(test)
endif()

# Build tiny-aes and lib-correct libs
add_subdirectory(libs)

# Build interface libs
add_subdirectory(interfaces)
