cmake_minimum_required(VERSION 3.15)

set(DEBUG_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/source/test_adc.c
  ${CMAKE_CURRENT_SOURCE_DIR}/source/test_can.c
  ${CMAKE_CURRENT_SOURCE_DIR}/source/test_gio.c
  ${CMAKE_CURRENT_SOURCE_DIR}/source/test_i2c.c
  ${CMAKE_CURRENT_SOURCE_DIR}/source/test_sci.c
  ${CMAKE_CURRENT_SOURCE_DIR}/source/test_spi.c
  ${CMAKE_CURRENT_SOURCE_DIR}/source/test_rtc.c
)

add_executable(debug-tool.out source/main.c)

target_link_options(debug-tool.out PRIVATE
    -specs=nosys.specs
    -Wl,-Map=${CMAKE_PROJECT_NAME}.map -T${LINKER_SCRIPT}
    -Wl,--print-memory-usage
)

if (DEBUG MATCHES 1)
    target_compile_options(debug-tool.out PRIVATE -Og -g -gdwarf-3 -gstrict-dwarf)
else()
    target_compile_options(debug-tool.out PRIVATE -O2)
endif()

target_compile_definitions(debug-tool.out PRIVATE
    ${BOARD_TYPE}
)

target_sources(debug-tool.out PUBLIC
  ${DEBUG_SOURCES}
)

target_include_directories(debug-tool.out PUBLIC
  ${CMAKE_BINARY_DIR}/toolchain/arm-none-eabi/include
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(debug-tool.out PRIVATE
  tiny-aes
  lib-correct
  obc-gs-interface
  ${HAL_LIB_OPTIMIZE}
  $<TARGET_OBJECTS:${HAL_LIB_NO_OPTIMIZE}>
)
