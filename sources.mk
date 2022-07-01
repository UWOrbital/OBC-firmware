# This is a top level makefile
# Used for building the final overall firmware with all components
# Maintained by CDH, only for building overall firmware, should not include individual subcomponent testing code

# All directories of source files that will be needed for firmware build
# Must list all subdirectores

SRC_DIRS :=
SRC_DIRS += hal/source
SRC_DIRS += obc_hal/source
SRC_DIRS += adcs/source
SRC_DIRS += cdh/source
SRC_DIRS += comms/source
SRC_DIRS += payload/source
