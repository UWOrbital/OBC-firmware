cmake_minimum_required(VERSION 3.15)

SET(INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/persistent
)

SET(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/obc_heap.c
    ${CMAKE_CURRENT_SOURCE_DIR}/persistent/obc_persistent.c
)

target_include_directories(OBC-firmware.out PUBLIC ${INCLUDES})
target_sources(OBC-firmware.out PUBLIC ${SOURCES})

target_include_directories(debug-tool.out PUBLIC ${INCLUDES})
target_sources(debug-tool.out PUBLIC ${SOURCES})
