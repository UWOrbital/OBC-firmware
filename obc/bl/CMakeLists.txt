cmake_minimum_required(VERSION 3.15)

set(BL_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/bl/source/bl_main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/bl/source/bl_flash.c
)

set(BL_INCLUDES
    ${CMAKE_BINARY_DIR}/toolchain/arm-none-eabi/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(OBC-bl.out PUBLIC ${BL_INCLUDES})

target_sources(OBC-bl.out PUBLIC
    ${CMAKE_SOURCE_DIR}/obc/bl/F021_Flash_API/source/Fapi_UserDefinedFunctions.c
)

add_library(f021_api STATIC IMPORTED)
set_property(TARGET f021_api PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/F021_Flash_API/F021_API_CortexR4_LE.lib)
target_include_directories(f021_api INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/F021_Flash_API/include)

add_library(f021_api_archive INTERFACE)
target_link_libraries(f021_api_archive INTERFACE "-Wl,--whole-archive" f021_api "-Wl,--no-whole-archive")

target_link_libraries(OBC-bl.out PRIVATE
    f021_api_archive
)
