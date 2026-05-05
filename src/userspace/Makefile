ASM     = nasm
CC      = i386-elf-gcc
LD      = i386-elf-ld
OBJCOPY = i386-elf-objcopy

CFLAGS  = -ffreestanding -nostdlib -m32 -I../../include/ -g -c

USER_DIR = ../../build/userspace

all: $(USER_DIR)/userland.exe

$(USER_DIR)/userland.exe: userland_entry.asm userland.c mach4.ld
	$(CC) $(CFLAGS) userland.c -o $(USER_DIR)/userland.o
	$(ASM) -f elf32 userland_entry.asm -o $(USER_DIR)/mach4_entry.o
	$(LD) -T mach4.ld -o $(USER_DIR)/userland.elf $(USER_DIR)/mach4_entry.o $(USER_DIR)/userland.o
	$(OBJCOPY) -O binary $(USER_DIR)/userland.elf $(USER_DIR)/userland.exe
