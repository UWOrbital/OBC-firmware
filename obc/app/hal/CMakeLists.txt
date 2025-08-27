cmake_minimum_required(VERSION 3.15)

set(INCLUDES

)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/app_intvecs.s
)

target_include_directories(${OUT_FILE_NAME} PUBLIC ${INCLUDES})
target_sources(${OUT_FILE_NAME} PUBLIC ${SOURCES})
