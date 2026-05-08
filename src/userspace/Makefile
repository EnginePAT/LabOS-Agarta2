ASM     = nasm
CC      = i386-elf-gcc
LD      = i386-elf-ld
OBJCOPY = i386-elf-objcopy

CFLAGS  = -ffreestanding -nostdlib -m32 -I../../include/ -g -c

USER_DIR = ../../build/userspace

all: $(USER_DIR)/userland.exe

$(USER_DIR)/userland.exe: userland_entry.asm userland.c mach4.ld
	$(CC) $(CFLAGS) userland.c -o $(USER_DIR)/userland.o
	$(CC) $(CFLAGS) core/syscall_handler.c -o $(USER_DIR)/syscall_handler.o
	$(CC) $(CFLAGS) core/syscall.c -o $(USER_DIR)/syscall.o
	$(CC) $(CFLAGS) core/handlers/handlers.c -o $(USER_DIR)/handlers.o
	$(CC) $(CFLAGS) ../kernel/core/vga/vga.c -o $(USER_DIR)/vga.o
	$(CC) $(CFLAGS) ../kernel/core/vga/serial.c -o $(USER_DIR)/serial.o
	$(CC) $(CFLAGS) ../util/util.c -o $(USER_DIR)/util.o

	$(ASM) -f elf32 userland_entry.asm -o $(USER_DIR)/mach4_entry.o

	$(LD) -T mach4.ld -o $(USER_DIR)/userland.elf $(USER_DIR)/mach4_entry.o $(USER_DIR)/userland.o $(USER_DIR)/syscall_handler.o $(USER_DIR)/syscall.o $(USER_DIR)/handlers.o $(USER_DIR)/vga.o $(USER_DIR)/serial.o $(USER_DIR)/util.o
	$(OBJCOPY) -O binary $(USER_DIR)/userland.elf $(USER_DIR)/userland.exe
