cmake_minimum_required(VERSION 3.15)

set(INCLUDES

)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/app_intvecs.s
)

target_include_directories(OBC-firmware.out PUBLIC ${INCLUDES})
target_sources(OBC-firmware.out PUBLIC ${SOURCES})
