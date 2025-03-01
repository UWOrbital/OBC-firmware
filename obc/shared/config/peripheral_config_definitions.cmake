# board_config_definitions.cmake

# Add Peripheral Configs

# Add setup specific periperal configs globally here
# Ex: if you connected vn100, add -DCONFIG_VN100 inside set
set(COMMON_PERIPHERAL_CONFIG)

# Board specifc configs - these should generally be kept as is unless the board is missing a component
if(BOARD_TYPE STREQUAL "RM46_LAUNCHPAD")
    set(PERIPHERAL_CONFIG ${COMMON_PERIPHERAL_CONFIG})
elseif(BOARD_TYPE STREQUAL "OBC_REVISION_1")
    set(PERIPHERAL_CONFIG ${COMMON_PERIPHERAL_CONFIG} -DCONFIG_DS3232 -DCONFIG_SD_CARD -DCONFIG_FRAM -DCONFIG_LM75BD)
elseif(BOARD_TYPE STREQUAL "OBC_REVISION_2")
    set(PERIPHERAL_CONFIG ${COMMON_PERIPHERAL_CONFIG} -DCONFIG_DS3232 -DCONFIG_SD_CARD -DCONFIG_FRAM -DCONFIG_LM75BD -DCONFIG_TPL5010)

else()
    message(FATAL_ERROR "Invalid BOARD_TYPE: ${BOARD_TYPE}. Board config not defined")
endif()
