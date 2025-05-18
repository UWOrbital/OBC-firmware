cmake_minimum_required(VERSION 3.15)

set(INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/obc_scheduler_config.c
    ${CMAKE_CURRENT_SOURCE_DIR}/framSdcIntegTest.c
)

target_include_directories(${OUT_FILE_NAME} PUBLIC ${INCLUDES})
target_sources(${OUT_FILE_NAME} PUBLIC ${SOURCES})

target_include_directories(debug-tool.out PUBLIC ${INCLUDES})
target_sources(debug-tool.out PUBLIC ${SOURCES})
