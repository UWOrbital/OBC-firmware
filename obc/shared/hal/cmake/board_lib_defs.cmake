cmake_minimum_required(VERSION 3.15)

if (${BOARD_TYPE} MATCHES RM46_LAUNCHPAD)
    set(HAL_LIB rm46-launchpad-hal)
elseif(${BOARD_TYPE} MATCHES OBC_REVISION_1)
    set(HAL_LIB obc-rev1-hal)
elseif(${BOARD_TYPE} MATCHES OBC_REVISION_2)
    set(HAL_LIB obc-rev2-hal)
else()
    message(FATAL_ERROR "Invalid board type: ${BOARD_TYPE}")
endif()

set(HAL_LIB_OPTIMIZE ${HAL_LIB}-optimize)
set(HAL_LIB_NO_OPTIMIZE ${HAL_LIB}-no-optimize)
