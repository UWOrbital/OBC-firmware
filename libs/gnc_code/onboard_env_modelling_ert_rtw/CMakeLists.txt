cmake_minimum_required(VERSION 3.15)

set(ONBOARD_ENV_MODELLING_LIB onboard-env-modelling)

add_library(${ONBOARD_ENV_MODELLING_LIB} STATIC
    source/onboard_env_modelling.c 
)

target_include_directories(${ONBOARD_ENV_MODELLING_LIB} PUBLIC
    include
)
