cmake_minimum_required(VERSION 3.15)

# ---------------------------------------------------------------
# Fetch GTest from github
# ---------------------------------------------------------------
include(FetchContent)

FetchContent_Declare(
wiringpi
GIT_REPOSITORY https://github.com/WiringPi/WiringPi.git
GIT_TAG        release-3.6
)
FetchContent_MakeAvailable(wiringpi)
add_library(wiringpi INTERFACE IMPORTED)
target_link_libraries(wiringpi INTERFACE IMPORTED)
# ---------------------------------------------------------------
