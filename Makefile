export BIN=OBC-firmware
export BUILD_DIR=build

.PHONY: all
all: cortex

.PHONY: posix
posix:		
	make -f make_files/Makefile.posix

.PHONY: cortex
cortex:	
	make -f make_files/Makefile.cortex

.PHONY: clean
clean:
	make -f make_files/Makefile.cortex clean
	make -f make_files/Makefile.posix clean