cmake_minimum_required(VERSION 3.15)

target_include_directories(obc-unit-tests PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/mocks
)

target_sources(obc-unit-tests PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/mocks/mock_rectangle.c
    ${CMAKE_CURRENT_SOURCE_DIR}/test_shape_container.c
)