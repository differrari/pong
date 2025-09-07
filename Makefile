ARCH       ?= aarch64-none-elf
CC         := $(ARCH)-gcc
CXX        := $(ARCH)-g++
LD         := $(ARCH)-ld

# Path to the [REDACTED] OS folder
OS_PATH ?= ../os

# Executable name (Due to a filesystem bug, keep the name at max 5 characters and extension at 3)
EXEC_NAME ?= pong.elf

# Path to the system's libshared. Static link only for now
STDINC ?= $(OS_PATH)/shared/
STDLIB ?= $(OS_PATH)/shared/libshared.a
CFLAGS ?= -ffreestanding -nostdlib -std=c99 -I$(STDINC) -O0
FS_PATH ?= $(OS_PATH)/fs/redos/user/$(EXEC_NAME)

.PHONY: dump

all:
	$(CC) $(CFLAGS) -c main.c -o main.o
	$(LD) -T linker.ld -o $(EXEC_NAME) main.o $(STDLIB)

run: all
	cp $(EXEC_NAME) $(FS_PATH)
	(cd $(OS_PATH); ./createfs; ./run_virt)

clean: 	
	rm main.o
	rm $(EXEC_NAME)

dump: all
	$(ARCH)-objdump -D $(EXEC_NAME) > dump