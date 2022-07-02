export BIN=OBC-firmware
export BUILD_DIR=build

.PHONY: all
all: rm46

.PHONY: posix
posix:		
	make -f make_files/Makefile.posix

.PHONY: rm46
rm46:	
	make -f make_files/Makefile.rm46

.PHONY: clean
clean:
	make -f make_files/Makefile.rm46 clean
	make -f make_files/Makefile.posix clean