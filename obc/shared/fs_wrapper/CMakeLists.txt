cmake_minimum_required(VERSION 3.15)

SET(INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}
)

SET (SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_reliance_fs.c
)

target_include_directories(OBC-firmware.out PUBLIC ${INCLUDES})
target_sources(OBC-firmware.out PUBLIC ${SOURCES})

target_include_directories(OBC-bl.out PUBLIC ${INCLUDES})
target_sources(OBC-bl.out PUBLIC ${SOURCES})

target_include_directories(debug-tool.out PUBLIC ${INCLUDES})
target_sources(debug-tool.out PUBLIC ${SOURCES})
