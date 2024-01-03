cmake_minimum_required(VERSION 3.15)

set(ONBOARD_ENV_MODELLING_LIB onboard_env_modelling)

add_library(${ONBOARD_ENV_MODELLING_LIB} STATIC
    source/onboad_env_modelling.c 
    source/ert_main.c
)

target_include_directories(${ONBOARD_ENV_MODELLING_LIB} PUBLIC
    include
)
