cmake_minimum_required(VERSION 3.15)

set(INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/metadata
)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/metadata.c
)

target_include_directories(OBC-firmware.out PUBLIC ${INCLUDES})
target_sources(OBC-firmware.out PUBLIC ${SOURCES})

target_include_directories(debug-tool.out PUBLIC ${INCLUDES})
target_sources(debug-tool.out PUBLIC ${SOURCES})
