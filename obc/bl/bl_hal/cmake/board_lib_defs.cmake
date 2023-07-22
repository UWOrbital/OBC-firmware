cmake_minimum_required(VERSION 3.15)

if (${BOARD_TYPE} MATCHES RM46_LAUNCHPAD)
    set(BL_HAL_LIB rm46-launchpad-bl-hal)
else()
    message(FATAL_ERROR "Invalid board type: ${BOARD_TYPE}")
endif()

set(BL_HAL_LIB_ARCHIVE ${HAL_LIB}-archive)
set(BL_HAL_LIB_NO_OPTIMIZE ${HAL_LIB}-no-optimize)
