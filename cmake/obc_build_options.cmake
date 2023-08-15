cmake_minimum_required(VERSION 3.15)

# Set default values for build options
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

if (NOT DEFINED CMD_POLICY)
    set(CMD_POLICY CMD_POLICY_RND)
endif()

if (NOT DEFINED COMMS_PHY)
    set(COMMS_PHY COMMS_PHY_NONE)
endif()

if (NOT DEFINED OBC_UART_BAUD_RATE)
    set(OBC_UART_BAUD_RATE 115200)
endif()

if (NOT DEFINED CSDC_DEMO_ENABLED)
    set(CSDC_DEMO_ENABLED 0)
endif()
