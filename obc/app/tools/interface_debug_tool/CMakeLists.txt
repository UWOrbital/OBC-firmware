cmake_minimum_required(VERSION 3.15)

set(DEBUG_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/source/main.c
  ${CMAKE_CURRENT_SOURCE_DIR}/source/test_adc.c
  ${CMAKE_CURRENT_SOURCE_DIR}/source/test_can.c
  ${CMAKE_CURRENT_SOURCE_DIR}/source/test_gio.c
  ${CMAKE_CURRENT_SOURCE_DIR}/source/test_i2c.c
  ${CMAKE_CURRENT_SOURCE_DIR}/source/test_sci.c
  ${CMAKE_CURRENT_SOURCE_DIR}/source/test_spi.c
  ${CMAKE_CURRENT_SOURCE_DIR}/source/test_lm75bd.c
  ${CMAKE_CURRENT_SOURCE_DIR}/source/test_rtc.c
)

SET(DEBUG_INCLUDES
  ${CMAKE_BINARY_DIR}/toolchain/arm-none-eabi/include
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_sources(debug-tool.out PUBLIC ${DEBUG_SOURCES})
target_include_directories(debug-tool.out PUBLIC ${DEBUG_INCLUDES})
