cmake_minimum_required(VERSION 3.15)

set(TOOLCHAIN_DIR ${CMAKE_BINARY_DIR}/toolchain)
set(TOOLCHAIN_TMP_DIR ${CMAKE_BINARY_DIR}/toolchain_tmp)
set(TOOLCHAIN_ZIP_FILE ${CMAKE_BINARY_DIR}/toolchain.tar.gz)

# Download GNU embedded toolchain
if(WIN32)
    set(TOOLCHAIN_URL "https://developer.arm.com/-/media/Files/downloads/gnu/11.2-2022.02/binrel/gcc-arm-11.2-2022.02-mingw-w64-i686-arm-none-eabi.zip")
elseif(UNIX)
    set(TOOLCHAIN_URL "https://developer.arm.com/-/media/Files/downloads/gnu/11.2-2022.02/binrel/gcc-arm-11.2-2022.02-x86_64-arm-none-eabi.tar.xz")
elseif(APPLE)
    set(TOOLCHAIN_URL "https://developer.arm.com/-/media/Files/downloads/gnu/11.2-2022.02/binrel/gcc-arm-11.2-2022.02-darwin-x86_64-arm-none-eabi.pkg")
else()
    message(FATAL_ERROR "Unsupported OS")
endif()

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
