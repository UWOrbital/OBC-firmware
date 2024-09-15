# Append current directory to CMAKE_MODULE_PATH for making device specific cmake modules visible
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

set(TOOLCHAIN_DIR ${CMAKE_BINARY_DIR}/toolchain)

# Target definition
set(CMAKE_SYSTEM_NAME  Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

if(NOT DEFINED SPECIFIC_CPU_FLAGS)
    #If not specified, don't crash
    set(SPECIFIC_CPU_FLAGS "")
endif()

#---------------------------------------------------------------------------------------
# Set toolchain paths
#---------------------------------------------------------------------------------------
set(TOOLCHAIN arm-none-eabi)
set(TOOLCHAIN_BIN_DIR ${TOOLCHAIN_DIR}/bin)
set(TOOLCHAIN_INC_DIR ${TOOLCHAIN_DIR}/${TOOLCHAIN}/include)
set(TOOLCHAIN_LIB_DIR ${TOOLCHAIN_DIR}/${TOOLCHAIN}/lib)

# Set system-dependent extensions
if(WIN32)
    set(TOOLCHAIN_EXT ".exe" )
else()
    set(TOOLCHAIN_EXT "" )
endif()

# Perform compiler test with static library
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

#---------------------------------------------------------------------------------------
# Set compiler/linker flags
#---------------------------------------------------------------------------------------
# ARM specific flags
add_compile_options(-mcpu=cortex-r4 -march=armv7-r -mtune=cortex-r4 -marm -mfpu=vfpv3-d16)

# Common flags
add_compile_options(-Wall -Wextra -Wno-unused-parameter -fstack-protector-strong -fstack-usage -fdump-ipa-cgraph -MMD)

# Conditional flag for C code
add_compile_options($<$<COMPILE_LANGUAGE:C>:-std=gnu99>)

# Linker flags
add_link_options(-mcpu=cortex-r4 -march=armv7-r -mtune=cortex-r4 -marm -mfpu=vfpv3-d16)

#---------------------------------------------------------------------------------------
# Set compilers
#---------------------------------------------------------------------------------------
set(CMAKE_C_COMPILER ${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN}-gcc${TOOLCHAIN_EXT} CACHE INTERNAL "C Compiler")
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN}-gcc${TOOLCHAIN_EXT} CACHE INTERNAL "ASM Compiler")

set(CMAKE_FIND_ROOT_PATH ${TOOLCHAIN_DIR}/${${TOOLCHAIN}} ${CMAKE_PREFIX_PATH})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
