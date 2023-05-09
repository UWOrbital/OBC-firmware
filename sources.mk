# This is a top level makefile
# Used for building the final overall firmware with all components
# Maintained by CDH, only for building overall firmware, should not include individual subcomponent testing code

# All directories of source files that will be needed for firmware build
# Must list all subdirectores

SRC_DIRS :=
SRC_DIRS += hal/source

# Drivers
SRC_DIRS += drivers/common/source
SRC_DIRS += drivers/ds3232/source
SRC_DIRS += drivers/fram/source
SRC_DIRS += drivers/lm75bd/source
SRC_DIRS += drivers/cc1120/source

SRC_DIRS += common/source
SRC_DIRS += adcs/source
SRC_DIRS += cdh/source
SRC_DIRS += comms/source
SRC_DIRS += eps/source
SRC_DIRS += payload/source

# Reliance Edge File System
SRC_DIRS += reliance_edge/fatfs_port
SRC_DIRS += reliance_edge/bdev
SRC_DIRS += reliance_edge/core/driver
SRC_DIRS += reliance_edge/fse
SRC_DIRS += reliance_edge/os/freertos/services
SRC_DIRS += reliance_edge/posix
SRC_DIRS += reliance_edge/util
SRC_DIRS += reliance_edge/projects/freertos_rm46/host

# Tiny AES
SRC_DIRS += tiny_aes
