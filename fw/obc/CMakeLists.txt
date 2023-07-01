cmake_minimum_required(VERSION 3.15)

target_include_directories(OBC-firmware.out PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/config
)

add_subdirectory(sys)
add_subdirectory(drivers)
add_subdirectory(hal)
add_subdirectory(services)
