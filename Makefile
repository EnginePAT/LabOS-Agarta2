#
# Build Tools
#
ASM=nasm
CC=i386-elf-gcc
LD=i386-elf-ld
QEMU=qemu-system-i386

E2MDIR=e2mkdir
E2MFS=mke2fs
E2CPY=e2cp

CFLAGS=-ffreestanding -nostdlib -m32 -Iinclude/ -g -c


#
# Directories
#
SRC_DIR=src
BUILD_DIR=build
BOOTLOADER_DIR=$(BUILD_DIR)/boot
KERNEL_DIR=$(BUILD_DIR)/kernel
USER_DIR=$(BUILD_DIR)/userspace
TOOLS_DIR=tools


#
# Image
#
all: mtools $(BUILD_DIR)/labos-agarta.img

$(BUILD_DIR)/labos-agarta.img: stage1 stage2 kernel | always
	@echo *** COPYING BOOTFS ***
	dd if=$(BOOTLOADER_DIR)/boot.bin of=$@ conv=notrunc
	./tools/mkfs.labfs $@
	./tools/mdir.labfs $@ boot /boot
	./tools/mcopy.labfs $@ config.cfg /config.cfg
	./tools/mcopy.labfs $@ $(BOOTLOADER_DIR)/stage2.bin /stage2.bin
	./tools/mcopy.labfs $@ $(KERNEL_DIR)/kernel.bin /boot/kernel.bin

# Create the EXT2 image, and copy/create files onto it
	@echo *** CREATING FILESYSTEM ***
	dd if=/dev/zero of=ext2.img bs=1M count=64
	mkfs.ext2 -b 1024 ext2.img
	$(E2MDIR) ext2.img:/boot
	$(E2MDIR) ext2.img:/usr
	$(E2MDIR) ext2.img:/bin
	$(E2MDIR) ext2.img:/lib
	$(E2MDIR) ext2.img:/home
	$(E2MDIR) ext2.img:/var
	$(E2MDIR) ext2.img:/tmp
	$(E2MDIR) ext2.img:/etc
	$(E2MDIR) ext2.img:/Frameworks
	$(E2MDIR) ext2.img:/Apps
	$(E2CPY) test.txt ext2.img:/test.txt
	$(E2CPY) LabOS-Mascot.bmp ext2.img:/etc/LabOS-Mascot.bmp


#
# Stage 1 Bootloader
#
stage1: $(BOOTLOADER_DIR)/boot.bin
	@echo *** BUILD: STAGE 1 BOOTLOADER ***

$(BOOTLOADER_DIR)/boot.bin: $(SRC_DIR)/bootloader/boot.asm | always
	$(ASM) $< -f bin -o $@

$(BOOTLOADER_DIR)/zeroes.bin: $(SRC_DIR)/bootloader/zeroes.asm | always
	$(ASM) $< -f bin -o $@


#
# Stage 2 Bootloader
#
stage2:	$(BOOTLOADER_DIR)/stage2.bin
	@echo *** BUILD: STAGE 2 BOOTLOADER ***

STAGE2_OBJS=$(BOOTLOADER_DIR)/stage2_entry.o $(BOOTLOADER_DIR)/stage2.o $(BOOTLOADER_DIR)/util.o \
	$(BOOTLOADER_DIR)/ata.o $(BOOTLOADER_DIR)/serial.o

$(BOOTLOADER_DIR)/stage2.bin: $(STAGE2_OBJS) | always
	$(LD) -T $(SRC_DIR)/linker.ld $^ -o $@ --oformat binary

$(BOOTLOADER_DIR)/stage2_entry.o: $(SRC_DIR)/bootloader/stage2/stage2_entry.asm | always
	$(ASM) $< -f elf32 -o $@

$(BOOTLOADER_DIR)/stage2.o: $(SRC_DIR)/bootloader/stage2/stage2.c | always
	$(CC) $(CFLAGS) $< -o $@

$(BOOTLOADER_DIR)/util.o: $(SRC_DIR)/util/util.c | always
	$(CC) $(CFLAGS) $< -o $@

$(BOOTLOADER_DIR)/ata.o: $(SRC_DIR)/bootloader/stage2/disk/ata.c | always
	$(CC) $(CFLAGS) $< -o $@

$(BOOTLOADER_DIR)/serial.o: $(SRC_DIR)/bootloader/stage2/serial.c | always
	$(CC) $(CFLAGS) $< -o $@

#
# Kernel
#
kernel:	$(KERNEL_DIR)/kernel.bin
	@echo *** BUILD: KERNEL ***

KERNEL_OBJS=$(KERNEL_DIR)/kernel_entry.o $(KERNEL_DIR)/kernel.o $(KERNEL_DIR)/util.o $(KERNEL_DIR)/vga.o $(KERNEL_DIR)/keyboard.o \
	$(KERNEL_DIR)/gdt.o $(KERNEL_DIR)/gdt_s.o $(KERNEL_DIR)/mem.o $(KERNEL_DIR)/idt_s.o $(KERNEL_DIR)/idt.o $(KERNEL_DIR)/pit.o \
	$(KERNEL_DIR)/ata.o $(KERNEL_DIR)/ext2.o $(KERNEL_DIR)/serial.o $(KERNEL_DIR)/shell.o $(KERNEL_DIR)/mouse.o $(KERNEL_DIR)/pmm.o \
	$(KERNEL_DIR)/vmm.o $(KERNEL_DIR)/memory.o $(USER_DIR)/userspace.o $(USER_DIR)/userspace_s.o $(USER_DIR)/syscall_handler.o \
	$(USER_DIR)/syscall_handler_s.o $(USER_DIR)/syscall.o

$(KERNEL_DIR)/kernel.bin: $(KERNEL_OBJS) | always
	$(LD) -T $(SRC_DIR)/kernel.ld $^ -o $@ --oformat binary

$(KERNEL_DIR)/kernel_entry.o: $(SRC_DIR)/kernel/kernel_entry.asm | always
	$(ASM) $< -f elf32 -o $@

$(KERNEL_DIR)/kernel.o: $(SRC_DIR)/kernel/kernel.c | always
	$(CC) $(CFLAGS) $< -o $@

$(KERNEL_DIR)/util.o: $(SRC_DIR)/util/util.c | always
	$(CC) $(CFLAGS) $< -o $@

$(KERNEL_DIR)/mem.o: $(SRC_DIR)/util/mem.c | always
	$(CC) $(CFLAGS) $< -o $@

$(KERNEL_DIR)/vga.o: $(SRC_DIR)/kernel/core/vga/vga.c | always
	$(CC) $(CFLAGS) $< -o $@

$(KERNEL_DIR)/serial.o: $(SRC_DIR)/kernel/core/vga/serial.c | always
	$(CC) $(CFLAGS) $< -o $@

$(KERNEL_DIR)/keyboard.o: $(SRC_DIR)/kernel/core/keyboard.c | always
	$(CC) $(CFLAGS) $< -o $@

$(KERNEL_DIR)/mouse.o: $(SRC_DIR)/kernel/core/mouse.c | always
	$(CC) $(CFLAGS) $< -o $@

$(KERNEL_DIR)/gdt.o: $(SRC_DIR)/kernel/core/mm/gdt.c | always
	$(CC) $(CFLAGS) $< -o $@

$(KERNEL_DIR)/gdt_s.o: $(SRC_DIR)/kernel/core/mm/gdt.asm | always
	$(ASM) $< -f elf32 -o $@

$(KERNEL_DIR)/idt.o: $(SRC_DIR)/kernel/core/mm/idt/idt.c | always
	$(CC) $(CFLAGS) $< -o $@

$(KERNEL_DIR)/idt_s.o: $(SRC_DIR)/kernel/core/mm/idt/idt.asm | always
	$(ASM) $< -f elf32 -o $@

$(KERNEL_DIR)/pit.o: $(SRC_DIR)/kernel/core/mm/pit.c | always
	$(CC) $(CFLAGS) $< -o $@

$(KERNEL_DIR)/ata.o: $(SRC_DIR)/kernel/disk/ata.c | always
	$(CC) $(CFLAGS) $< -o $@

$(KERNEL_DIR)/ext2.o: $(SRC_DIR)/kernel/fs/ext2.c | always
	$(CC) $(CFLAGS) $< -o $@

$(KERNEL_DIR)/shell.o: $(SRC_DIR)/kernel/core/shell.c | always
	$(CC) $(CFLAGS) $< -o $@

$(KERNEL_DIR)/pmm.o: $(SRC_DIR)/kernel/core/mm/pmm.c | always
	$(CC) $(CFLAGS) $< -o $@

$(KERNEL_DIR)/vmm.o: $(SRC_DIR)/kernel/core/mm/vmm.c | always
	$(CC) $(CFLAGS) $< -o $@

$(KERNEL_DIR)/memory.o: $(SRC_DIR)/kernel/core/mm/memory.c | always
	$(CC) $(CFLAGS) $< -o $@

# Userpsace (integrated into kernel)
$(USER_DIR)/userspace.o: $(SRC_DIR)/userspace/userspace.c | always
	$(CC) $(CFLAGS) $< -o $@

$(USER_DIR)/userspace_s.o: $(SRC_DIR)/userspace/userspace.asm | always
	$(ASM) $< -f elf32 -o $@

$(USER_DIR)/syscall_handler.o: $(SRC_DIR)/userspace/core/syscall_handler.c | always
	$(CC) $(CFLAGS) $< -o $@

$(USER_DIR)/syscall_handler_s.o: $(SRC_DIR)/userspace/core/syscall_handler.asm | always
	$(ASM) $< -f elf32 -o $@

$(USER_DIR)/syscall.o: $(SRC_DIR)/userspace/core/syscall.c | always
	$(CC) $(CFLAGS) $< -o $@

#
# Tools
#
mtools: $(TOOLS_DIR)/mkfs.labfs $(TOOLS_DIR)/mcopy.labfs $(TOOLS_DIR)/mdir.labfs $(TOOLS_DIR)/mdump.labfs
	@echo *** BUILD: LABFS MTOOLS ***
	@echo \n

$(TOOLS_DIR)/mkfs.labfs: $(TOOLS_DIR)/mkfs.labfs.c
	gcc $< -o $@

$(TOOLS_DIR)/mcopy.labfs: $(TOOLS_DIR)/mcopy.labfs.c
	gcc $< -o $@

$(TOOLS_DIR)/mdir.labfs: $(TOOLS_DIR)/mdir.labfs.c
	gcc $< -o $@

$(TOOLS_DIR)/mdump.labfs: $(TOOLS_DIR)/mdump.labfs.c
	gcc $< -o $@


#
# Clean
#
clean:
	rm -rf $(BUILD_DIR)/*
	rm -rf ext2.img
	rm -rf $(TOOLS_DIR)/mcopy.labfs
	rm -rf $(TOOLS_DIR)/mkfs.labfs
	rm -rf $(TOOLS_DIR)/mdir.labfs
	rm -rf $(TOOLS_DIR)/mdump.labfs


#
# Always
#
always:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BOOTLOADER_DIR)
	mkdir -p $(KERNEL_DIR)
	mkdir -p $(USER_DIR)


#
# Run
#
run:
	$(QEMU) \
		-drive file=$(BUILD_DIR)/labos-agarta.img,format=raw,if=ide,index=0 \
		-drive file=ext2.img,format=raw,if=ide,index=1 \
		-serial stdio
