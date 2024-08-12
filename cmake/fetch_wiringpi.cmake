cmake_minimum_required(VERSION 3.15)

# ---------------------------------------------------------------
# Fetch WiringPi from github
# ---------------------------------------------------------------
include(FetchContent)

FetchContent_Declare(
wiringpi
GIT_REPOSITORY https://github.com/WiringPi/WiringPi.git
GIT_TAG        release-3.6
)
FetchContent_MakeAvailable(wiringpi)

# ---------------------------------------------------------------
# Manually building wiringPi
# ---------------------------------------------------------------
add_custom_target(
    wiringpi-build
    COMMAND ${CMAKE_COMMAND} -E chdir ${wiringpi_BINARY_DIR} make
    WORKING_DIRECTORY ${wiringpi_SOURCE_DIR}
)

add_library(wiringpi INTERFACE)
target_include_directories(wiringpi INTERFACE ${wiringpi_SOURCE_DIR})
target_link_libraries(wiringpi INTERFACE ${wiringpi_BINARY_DIR}/wiringPi)
# ---------------------------------------------------------------
