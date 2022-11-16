# This is a top level makefile
# Used for building the final overall firmware with all components
# Maintained by CDH, only for building overall firmware, should not include individual subcomponent testing code

# All directories of source files that will be needed for firmware build
# Must list all subdirectores

SRC_DIRS :=
SRC_DIRS += hal/source
SRC_DIRS += drivers/source
SRC_DIRS += adcs/source
SRC_DIRS += cdh/source
SRC_DIRS += comms/source
SRC_DIRS += payload/source
# SRC_DIRS += fatfs/src
SRC_DIRS += fatfs/port

SRC_DIRS += reliance-edge/bdev
SRC_DIRS += reliance-edge/core/driver
SRC_DIRS += reliance-edge/fse
SRC_DIRS += reliance-edge/os/freertos/services
SRC_DIRS += reliance-edge/posix
SRC_DIRS += reliance-edge/util
# SRC_DIRS += reliance-edge/util/bitmap.c
# SRC_DIRS += reliance-edge/util/crc.c
# SRC_DIRS += reliance-edge/util/memory.c
# SRC_DIRS += reliance-edge/util/namelen.c
# SRC_DIRS += reliance-edge/util/sign.c
# SRC_DIRS += reliance-edge/util/string.c
SRC_DIRS += reliance-edge/projects/newproj/host
