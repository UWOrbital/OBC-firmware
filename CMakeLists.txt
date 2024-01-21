cmake_minimum_required(VERSION 3.15)


# Set the toolchain file before the project is declared
if(${CMAKE_BUILD_TYPE} MATCHES OBC)
    # Download the toolchain if it doesn't exist
    if(NOT EXISTS ${CMAKE_BINARY_DIR}/toolchain)
        include(${CMAKE_SOURCE_DIR}/cmake/download_arm_toolchain.cmake)
    endif()
    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/toolchain_arm_none_eabi.cmake)
elseif(${CMAKE_BUILD_TYPE} MATCHES GS)
    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/toolchain_ground_station_gcc.cmake)
elseif(${CMAKE_BUILD_TYPE} MATCHES Test)
    include(${CMAKE_SOURCE_DIR}/cmake/fetch_googletest.cmake)
    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/toolchain_linux_gcc.cmake)
else()
    message(FATAL_ERROR "Invalid build type: ${CMAKE_BUILD_TYPE}")
endif()

project(OrbitalSw)

if(${CMAKE_BUILD_TYPE} MATCHES OBC)
    set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    enable_language(C ASM)
    add_subdirectory(obc)
endif()

if(${CMAKE_BUILD_TYPE} MATCHES GS)
    set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    enable_language(C CXX)
    add_subdirectory(gs)
endif()

if(${CMAKE_BUILD_TYPE} MATCHES Test)
    enable_language(C CXX)
    enable_testing()
    add_subdirectory(test)
    add_compile_options(-m32)

    add_link_options(-m32)
    set(CMAKE_VERBOSE_MAKEFILE ON)
# Set compiler flags for 32-bit
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")

# Set linker flags for 32-bit
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -m32")

# Set shared library flags for 32 bits
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -m32")
set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -m32")
endif()

# Build tiny-aes and lib-correct libs
add_subdirectory(libs)

# Build interface libs
add_subdirectory(interfaces)
