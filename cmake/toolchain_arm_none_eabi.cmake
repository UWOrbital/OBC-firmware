# Append current directory to CMAKE_MODULE_PATH for making device specific cmake modules visible
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

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
target_compile_options(OBC-firmware.out PRIVATE -mcpu=cortex-r4 -march=armv7-r -mtune=cortex-r4 -marm -mfpu=vfpv3-d16)
target_compile_options(OBC-firmware.out PRIVATE -Og -g -gdwarf-3 -gstrict-dwarf -Wall -Wextra -Wno-unused-parameter -fstack-usage -fdump-ipa-cgraph -MMD -std=gnu99)
target_compile_definitions(OBC-firmware.out PRIVATE DEBUG RM46_LAUNCHPAD LOG_DEFAULT_OUTPUT_LOCATION=LOG_TO_UART LOG_DEFAULT_LEVEL=LOG_TRACE)
target_link_options(OBC-firmware.out PRIVATE -Wl,--gc-sections --specs=nosys.specs -Wl,-Map=${CMAKE_PROJECT_NAME}.map -T ${LINKER_SCRIPT})

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
