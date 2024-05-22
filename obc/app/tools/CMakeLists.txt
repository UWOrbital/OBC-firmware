cmake_minimum_required(VERSION 3.15)

# For power on test
set(INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}/power_on_test
)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/power_on_test/power_on_test.c
)

target_include_directories(OBC-firmware.out PUBLIC ${INCLUDES})
target_sources(OBC-firmware.out PUBLIC ${SOURCES})

target_include_directories(debug-tool.out PUBLIC ${INCLUDES})
target_sources(debug-tool.out PUBLIC ${SOURCES})

# For interface debug tool
set(DEBUG_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/interface_debug_tool/source/main.c
  ${CMAKE_CURRENT_SOURCE_DIR}/interface_debug_tool/source/test_adc.c
  ${CMAKE_CURRENT_SOURCE_DIR}/interface_debug_tool/source/test_can.c
  ${CMAKE_CURRENT_SOURCE_DIR}/interface_debug_tool/source/test_gio.c
  ${CMAKE_CURRENT_SOURCE_DIR}/interface_debug_tool/source/test_i2c.c
  ${CMAKE_CURRENT_SOURCE_DIR}/interface_debug_tool/source/test_sci.c
  ${CMAKE_CURRENT_SOURCE_DIR}/interface_debug_tool/source/test_spi.c
  ${CMAKE_CURRENT_SOURCE_DIR}/interface_debug_tool/source/test_lm75bd.c
)

SET(DEBUG_INCLUDES
  ${CMAKE_BINARY_DIR}/toolchain/arm-none-eabi/include
  ${CMAKE_CURRENT_SOURCE_DIR}/interface_debug_tool/include
)

target_sources(debug-tool.out PUBLIC ${DEBUG_SOURCES})
target_include_directories(debug-tool.out PUBLIC ${DEBUG_INCLUDES})
