cmake_minimum_required(VERSION 3.15)

SET(INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_include_directories(${OUT_FILE_NAME} PUBLIC ${INCLUDES})
target_include_directories(OBC-bl.out PUBLIC ${INCLUDES})
target_include_directories(debug-tool.out PUBLIC ${INCLUDES})
