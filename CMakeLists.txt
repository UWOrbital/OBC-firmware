cmake_minimum_required(VERSION 3.15)

# Set the toolchain file before the project is declared
if(${CMAKE_BUILD_TYPE} MATCHES OBC)
    # Download the toolchain if it doesn't exist
    if(NOT EXISTS ${CMAKE_BINARY_DIR}/toolchain)
        include(${CMAKE_SOURCE_DIR}/cmake/download_arm_toolchain.cmake)
    endif()
    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/toolchain_arm_none_eabi.cmake)
elseif(${CMAKE_BUILD_TYPE} MATCHES GS)

elseif(${CMAKE_BUILD_TYPE} MATCHES Test)
    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/toolchain_linux_gcc.cmake)
else()
    message(FATAL_ERROR "Invalid build type: ${CMAKE_BUILD_TYPE}")
endif()

project(OrbitalSw)

if(${CMAKE_BUILD_TYPE} MATCHES GS)
    enable_language(C CXX)
    add_subdirectory(gs)
endif()

if(${CMAKE_BUILD_TYPE} MATCHES OBC)
    enable_language(C ASM)
    add_subdirectory(obc)
endif()

if(${CMAKE_BUILD_TYPE} MATCHES Test)
    enable_language(C CXX)
    add_subdirectory(test)
endif()

# Build tiny-aes and lib-correct libs
add_subdirectory(libs)

# Build interface libs
add_subdirectory(interfaces)
