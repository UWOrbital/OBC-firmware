cmake_minimum_required (VERSION 3.21)

project(OBCExamples LANGUAGES C)

# path to current source dir
set (path ${CMAKE_CURRENT_SOURCE_DIR})

set (EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build_example)

cmake_path(GET path PARENT_PATH parentPath)
cmake_path (GET parentPath PARENT_PATH grandparentPath)

#message("Current path is \"${path}\"")
#message("GrandParent path is \"${grandparentPath}\"")

set (BOARD_TYPE "-DRM46_LAUNCHPAD")
set (CMAKE_EXE_LINKER_FLAGS "--specs=nosys.specs")
set (CMAKE_C_COMPILER "${grandparentPath}/build_arm/toolchain/bin/arm-none-eabi-gcc")



if (${CMAKE_BUILD_TYPE} MATCHES DMA_SPI)

set (HEADERS
${parentPath}/sys/logging/obc_logging.h
${parentPath}/drivers/rm46/obc_sci_io.h
${parentPath}/sys/print/obc_print.h
${parentPath}/sys/obc_errors.h
${parentPath}/drivers/rm46/obc_i2c_io.h
${parentPath}/drivers/rm46/obc_spi_io.h
${parentPath}/hal/launchpad/include/sys_dma.h
${parentPath}/hal/launchpad/include/mibspi.h
${parentPath}/drivers/rm46/obc_spi_dma.h
${parentPath}/drivers/rm46/obc_privilege.h
${parentPath}/hal/launchpad/include/FreeRTOS.h
${parentPath}/hal/launchpad/include/os_task.h
${parentPath}/hal/launchpad/include/sys_common.h
${parentPath}/hal/launchpad/include/sys_core.h
${parentPath}/hal/launchpad/include/gio.h
${parentPath}/hal/launchpad/include/sci.h
${parentPath}/hal/launchpad/include/i2c.h
${parentPath}/hal/launchpad/include/spi.h
${parentPath}/hal/launchpad/include/can.h
${parentPath}/hal/launchpad/include/het.h
${parentPath}/hal/launchpad/include/system.h
)

set (INCLUDE_DIRS
${parentPath}/sys/print
${parentPath}/sys/logging
${parentPath}/sys
${parentPath}/drivers/rm46
${parentPath}/hal/launchpad
${parentPath}/hal/launchpad/include
)
endif()



message ("Current source dir is \"${CMAKE_CURRENT_SOURCE_DIR}\"")

get_filename_component(INCLUDE_DIRS_ABS "${INCLUDE_DIRS}" ABSOLUTE)

message ("Absolute path is \"${INCLUDE_DIRS_ABS}\"")


#message("Current HEADERS are \"${HEADERS}\"")


set (EXAMPLE_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/main.c)


add_executable (OBC-example.out ${EXAMPLE_SOURCE} ${HEADERS})


target_link_libraries(OBC-example.out ${HEADERS})
target_include_directories(OBC-example.out PRIVATE ${INCLUDE_DIRS_ABS})
#target_include_directories (OBC-example.out PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})



#target_compile_options(OBC-example.out PUBLIC -c -mcpu=cortex-r4 -march=armv7-r -mtune=cortex-r4 -marm -mfpu=vfpv3-d16 ${BOARD_TYPE} -O0 -g -gdwarf-3 -gstrict-dwarf)
