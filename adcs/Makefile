# This is the makefile for individual subteam component,
# To allow for build and testing of individual components
# Maintained by individual subteam, not called when building for overall firmware

# Makefile template

include_dirs := -I.
include_dirs += -I../../FreeRTOS/FreeRTOS/Source/include
include_dirs += -I../../FreeRTOS/FreeRTOS/Source/portable/ThirdParty/GCC/Posix

# Include all user source files in the current dir
my_source_files := $(wildcard *.c)

# Include FreeRTOS source files from sibling dir
source_files := $(wildcard ../../FreeRTOS/FreeRTOS/Source/*.c)
source_files += $(wildcard ../../FreeRTOS/FreeRTOS/Source/portable/MemMang/heap_3.c)
source_files += $(wildcard ../../FreeRTOS/FreeRTOS/Source/portable/ThirdParty/GCC/Posix/*.c)
source_files += $(wildcard ../../FreeRTOS/FreeRTOS/Source/portable/ThirdParty/GCC/Posix/utils/*.c)

# FreeRTOS Linux Port requires to have some boiler plate hooks called...
source_files += ../comms/posix_required_fns.c

build: $(source_files)
	# Compile with debug symbols
	# FreeRTOS also expects pthread to be linked
	gcc -Og -g $(include_dirs) $(source_files) $(my_source_files) -lpthread -o a.out

run: build
	./a.out