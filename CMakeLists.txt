cmake_minimum_required(VERSION 3.15)

if (CMAKE_BUILD_TYPE MATCHES Test)
    include(cmake/fetch_googletest.cmake)
    enable_testing()
endif()

set(TOOLCHAIN_DIR ${CMAKE_BINARY_DIR}/toolchain)

if(${CMAKE_BUILD_TYPE} MATCHES Firmware AND NOT EXISTS ${TOOLCHAIN_DIR})
    include(cmake/download_arm_toolchain.cmake)
endif()

if(CMAKE_BUILD_TYPE MATCHES Firmware)
    # Set default values for firmware build options
    include(cmake/fw_build_options.cmake)

    if(${BOARD_TYPE} MATCHES RM46_LAUNCHPAD)
        set(LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/fw/obc/hal/launchpad/source/sys_link.ld")
    elseif(${BOARD_TYPE} MATCHES OBC_REVISION_1)
        set(LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/fw/obc/hal/obc_rev1/source/sys_link.ld")
    elseif(${BOARD_TYPE} MATCHES OBC_REVISION_2)
        set(LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/fw/obc/hal/obc_rev2/source/sys_link.ld")
    else()
        message(FATAL_ERROR "Invalid board type: ${BOARD_TYPE}")
    endif()

    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/toolchain_arm_none_eabi.cmake")

    project(OBC-firmware LANGUAGES C ASM)
    add_executable(OBC-firmware.out main.c)
elseif(CMAKE_BUILD_TYPE MATCHES Test)
    project(OBC-firmware-tests LANGUAGES CXX C)
else()
    message(FATAL_ERROR "Invalid build type: ${CMAKE_BUILD_TYPE}")
endif()

if (CMAKE_BUILD_TYPE MATCHES Test)
    add_subdirectory(fw/test)
elseif(CMAKE_BUILD_TYPE MATCHES Firmware)
    if (DEBUG MATCHES 1)
        # Higher optimization levels break firmware for some reason
        target_compile_options(OBC-firmware.out PRIVATE -O0 -g -gdwarf-3 -gstrict-dwarf)
    else()
        target_compile_options(OBC-firmware.out PRIVATE -O0)
    endif()

    target_compile_definitions(OBC-firmware.out PRIVATE
        ${BOARD_TYPE}
        LOG_DEFAULT_OUTPUT_LOCATION=${LOG_DEFAULT_OUTPUT_LOCATION}
        LOG_DEFAULT_LEVEL=${LOG_DEFAULT_LEVEL}
        DEBUG=${DEBUG}
        CMD_POLICY=${CMD_POLICY}
        COMMS_PHY=${COMMS_PHY}
        OBC_UART_BAUD_RATE=${OBC_UART_BAUD_RATE}
        CSDC_DEMO_ENABLED=${CSDC_DEMO_ENABLED}
    )

    include_directories(${TOOLCHAIN_DIR}/arm-none-eabi/include)

    add_subdirectory(fw)

    add_subdirectory(libs)
    target_link_libraries(OBC-firmware.out PRIVATE
        tiny-aes
        lib-correct
        obc-gs-interface
    )

endif()
