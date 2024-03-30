cmake_minimum_required(VERSION 3.15)

set(INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/fs_wrapper
    ${CMAKE_CURRENT_SOURCE_DIR}/logging
    ${CMAKE_CURRENT_SOURCE_DIR}/persistent
    ${CMAKE_CURRENT_SOURCE_DIR}/print
    ${CMAKE_CURRENT_SOURCE_DIR}/state
    ${CMAKE_CURRENT_SOURCE_DIR}/time
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/fs_wrapper/obc_reliance_fs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/print/obc_print.c
    ${CMAKE_CURRENT_SOURCE_DIR}/persistent/obc_persistent.c
    ${CMAKE_CURRENT_SOURCE_DIR}/state/obc_state.c
    ${CMAKE_CURRENT_SOURCE_DIR}/time/obc_time.c
    ${CMAKE_CURRENT_SOURCE_DIR}/time/obc_time_utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/obc_heap.c
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/obc_crc.c
)


target_include_directories(OBC-firmware.out PUBLIC ${INCLUDES})
target_sources(OBC-firmware.out PUBLIC ${SOURCES})

target_include_directories(debug-tool.out PUBLIC ${INCLUDES})
target_sources(debug-tool.out PUBLIC ${SOURCES})
