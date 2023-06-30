cmake_minimum_required(VERSION 3.15)

target_include_directories(OBC-firmware.out PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/fs_utils
    ${CMAKE_CURRENT_SOURCE_DIR}/logging
    ${CMAKE_CURRENT_SOURCE_DIR}/persistent
    ${CMAKE_CURRENT_SOURCE_DIR}/state
    ${CMAKE_CURRENT_SOURCE_DIR}/time
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
)

target_sources(OBC-firmware.out PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/fs_utils/obc_fs_utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/logging/obc_logging.c
    ${CMAKE_CURRENT_SOURCE_DIR}/persistent/obc_persistent_store.c
    ${CMAKE_CURRENT_SOURCE_DIR}/state/obc_state.c
    ${CMAKE_CURRENT_SOURCE_DIR}/time/obc_time.c
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/obc_heap.c
)
