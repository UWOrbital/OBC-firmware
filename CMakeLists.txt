cmake_minimum_required(VERSION 3.15)

# Download GNU embedded toolchain
if(WIN32)
    set(TOOLCHAIN_URL "https://developer.arm.com/-/media/Files/downloads/gnu/11.2-2022.02/binrel/gcc-arm-11.2-2022.02-mingw-w64-i686-arm-none-eabi.zip?rev=d8a8b27ec59c4598867b64d94c9d3deb&hash=0D1BFBFE08CBE5B2CA0F3E34FE082BC23B1BEDD8")
elseif(APPLE)
    set(TOOLCHAIN_URL "https://developer.arm.com/-/media/Files/downloads/gnu/11.2-2022.02/binrel/gcc-arm-11.2-2022.02-x86_64-arm-none-eabi.tar.xz?rev=99a2bce6f4464be08eca01eda13e4e96&hash=C1BA53B2F16F00A518C5781098C2E56144EA3FCD")
elseif(UNIX)
    set(TOOLCHAIN_URL "https://developer.arm.com/-/media/Files/downloads/gnu/11.2-2022.02/binrel/gcc-arm-11.2-2022.02-darwin-x86_64-arm-none-eabi.pkg?rev=882c46dfe9624046a2cadd7e10a55d0b&hash=1DEE30CD20177B730B635948CA6AB1A4A500D16B")
else()
    message(FATAL_ERROR "Unsupported OS")
endif()

set(TOOLCHAIN_DIR ${CMAKE_BINARY_DIR}/toolchain)
set(TOOLCHAIN_TMP_DIR ${CMAKE_BINARY_DIR}/toolchain_tmp)
set(TOOLCHAIN_ZIP_FILE ${CMAKE_BINARY_DIR}/toolchain.tar.gz)

if(NOT EXISTS ${TOOLCHAIN_DIR})    
    # Download the toolchain
    file(DOWNLOAD 
        ${TOOLCHAIN_URL} 
        ${TOOLCHAIN_ZIP_FILE}
        SHOW_PROGRESS)

    # Check for download errors
    if(NOT EXISTS ${TOOLCHAIN_ZIP_FILE})
        message(FATAL_ERROR "Failed to download toolchain")
    endif()

    # Extract the toolchain
    execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory "${TOOLCHAIN_TMP_DIR}")

    execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf "${TOOLCHAIN_ZIP_FILE}"
        WORKING_DIRECTORY "${TOOLCHAIN_TMP_DIR}")

    file(GLOB TOOLCHAIN_TMP_DIR_CONTENTS "${TOOLCHAIN_TMP_DIR}/*")
    list(GET TOOLCHAIN_TMP_DIR_CONTENTS 0 INNER_DIR_PATH)

    file(RENAME ${INNER_DIR_PATH} ${TOOLCHAIN_DIR})

    # Remove the temporary directory
    execute_process(COMMAND ${CMAKE_COMMAND} -E remove_directory "${TOOLCHAIN_TMP_DIR}")

endif()

if (NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Firmware)
endif()

if (NOT DEFINED DEBUG)
    set(DEBUG 1)
endif()

if (NOT DEFINED BOARD_TYPE)
    set(BOARD_TYPE RM46_LAUNCHPAD)
endif()

if (NOT DEFINED LOG_DEFAULT_OUTPUT_LOCATION)
    set(LOG_DEFAULT_OUTPUT_LOCATION LOG_TO_UART)
endif()

if (NOT DEFINED LOG_DEFAULT_LEVEL)
    set(LOG_DEFAULT_LEVEL LOG_TRACE)
endif()

if(CMAKE_BUILD_TYPE MATCHES Firmware)
    set(LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/hal/source/sys_link.ld")
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/toolchain_arm_none_eabi.cmake")

    project(OBC-firmware LANGUAGES C ASM)
    
    set(PROJECT_EXTENSION out)
    set(EXECUTABLE_NAME ${PROJECT_NAME}.${PROJECT_EXTENSION})
    
    set(SOURCES main.c)

else()
    message(FATAL_ERROR "Invalid build type: ${CMAKE_BUILD_TYPE}")
endif()

add_executable(${EXECUTABLE_NAME} ${SOURCES})

if(CMAKE_BUILD_TYPE MATCHES Firmware)
    if (DEBUG MATCHES 1)
        message(STATUS "Debug build")
        target_compile_options(OBC-firmware.out PRIVATE -Og -g -gdwarf-3 -gstrict-dwarf)
    else()
        message(STATUS "Release build")
        target_compile_options(OBC-firmware.out PRIVATE -O0)
    endif()

    if (BOARD_TYPE MATCHES RM46_LAUNCHPAD)
        message(STATUS "Using RM46 Launchpad")
        target_compile_definitions(OBC-firmware.out PRIVATE RM46_LAUNCHPAD)
    elseif (BOARD_TYPE MATCHES OBC_REVISION_1)
        message(STATUS "Using OBC Revision 1")
        target_compile_definitions(OBC-firmware.out PRIVATE OBC_REVISION_1)
    elseif (BOARD_TYPE MATCHES OBC_REVISION_2)
        message(STATUS "Using OBC Revision 2")
        target_compile_definitions(OBC-firmware.out PRIVATE OBC_REVISION_2)
    else()
        message(FATAL_ERROR "Invalid board type: ${BOARD_TYPE}")
    endif()

    target_compile_definitions(OBC-firmware.out PRIVATE LOG_DEFAULT_OUTPUT_LOCATION=${LOG_DEFAULT_OUTPUT_LOCATION} LOG_DEFAULT_LEVEL=${LOG_DEFAULT_LEVEL} DEBUG=${DEBUG})

    include_directories(${TOOLCHAIN_DIR}/arm-none-eabi/include)
    add_subdirectory(adcs)
    add_subdirectory(cdh)
    add_subdirectory(common)
    add_subdirectory(comms)
    add_subdirectory(drivers)
    add_subdirectory(eps)
    add_subdirectory(hal)
    add_subdirectory(libcorrect)
    add_subdirectory(payload)
    add_subdirectory(reliance_edge)
    add_subdirectory(tiny_aes)
endif()
