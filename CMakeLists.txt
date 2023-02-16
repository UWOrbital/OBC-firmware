cmake_minimum_required(VERSION 3.15)

if(CMAKE_BUILD_TYPE MATCHES Firmware)
    set(CMAKE_TOOLCHAIN_FILE cmake/toolchain_arm_none_eabi.cmake)
    set(LINKER_SCRIPT ../hal/source/sys_link.ld)
    set(PROJECT_EXTENSION out)

    file (STRINGS ".env" TOOLCHAIN_PREFIX)

    project(OBC-firmware LANGUAGES C ASM)

    set(EXECUTABLE_NAME ${PROJECT_NAME}.${PROJECT_EXTENSION})
    set(SOURCES main.c)
elseif(CMAKE_BUILD_TYPE MATCHES Test)
    project(obc-unit-tests LANGUAGES C)
    set(EXECUTABLE_NAME ${PROJECT_NAME})
    include_directories(examples/unit_test_demo)
    set(SOURCES
        examples/unit_test_demo/shape_container.c
    )
else()
    message(FATAL_ERROR "Invalid build type: ${CMAKE_BUILD_TYPE}")
endif()

add_executable(${EXECUTABLE_NAME} ${SOURCES})

if(CMAKE_BUILD_TYPE MATCHES Firmware)
    include_directories(${TOOLCHAIN_PREFIX}/arm-none-eabi/include)
    add_subdirectory(adcs)
    add_subdirectory(cdh)
    add_subdirectory(common)
    add_subdirectory(comms)
    add_subdirectory(drivers)
    add_subdirectory(eps)
    add_subdirectory(hal)
    add_subdirectory(payload)
elseif(CMAKE_BUILD_TYPE MATCHES Test)
    add_subdirectory(tests)
    add_subdirectory(unity)
    add_subdirectory(cmock)
endif()

