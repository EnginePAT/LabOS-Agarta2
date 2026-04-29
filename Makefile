#
# Build Tools
#
ASM=nasm
CC=i386-elf-gcc
LD=i386-elf-ld
QEMU=qemu-system-i386

CFLAGS=-ffreestanding -nostdlib -m32 -Iinclude/ -g -c


#
# Directories
#
SRC_DIR=src
BUILD_DIR=build
BOOTLOADER_DIR=$(BUILD_DIR)/boot
KERNEL_DIR=$(BUILD_DIR)/kernel
TOOLS_DIR=tools


#
# Image
#
all: mtools $(BUILD_DIR)/labos-agarta.img

$(BUILD_DIR)/labos-agarta.img: stage1 stage2 kernel | always
	dd if=$(BOOTLOADER_DIR)/boot.bin of=$@ conv=notrunc
	./tools/mkfs.labfs $@
	./tools/mdir.labfs $@ boot /boot
	./tools/mcopy.labfs $@ config.cfg /config.cfg
	./tools/mcopy.labfs $@ $(BOOTLOADER_DIR)/stage2.bin /stage2.bin
	./tools/mcopy.labfs $@ $(KERNEL_DIR)/kernel.bin /boot/kernel.bin

# Create the EXT2 image, and copy/create files onto it
	mke2fs -t ext2 ext2.img 32M
	e2mkdir ext2.img:/boot
	e2mkdir ext2.img:/usr
	e2mkdir ext2.img:/bin
	e2mkdir ext2.img:/lib
	e2mkdir ext2.img:/home
	e2mkdir ext2.img:/var
	e2mkdir ext2.img:/tmp
	e2mkdir ext2.img:/etc
	e2mkdir ext2.img:/Frameworks
	e2mkdir ext2.img:/Apps
	e2cp test.txt ext2.img:/test.txt
	e2cp LabOS-Mascot.bmp ext2.img:/etc/LabOS-Mascot.bmp


#
# Stage 1 Bootloader
#
stage1: $(BOOTLOADER_DIR)/boot.bin

$(BOOTLOADER_DIR)/boot.bin: $(SRC_DIR)/bootloader/boot.asm | always
	$(ASM) $< -f bin -o $@

$(BOOTLOADER_DIR)/zeroes.bin: $(SRC_DIR)/bootloader/zeroes.asm | always
	$(ASM) $< -f bin -o $@


#
# Stage 2 Bootloader
#
stage2: $(BOOTLOADER_DIR)/stage2.bin

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
kernel: $(KERNEL_DIR)/kernel.bin

KERNEL_OBJS=$(KERNEL_DIR)/kernel_entry.o $(KERNEL_DIR)/kernel.o $(KERNEL_DIR)/util.o $(KERNEL_DIR)/vga.o $(KERNEL_DIR)/keyboard.o \
	$(KERNEL_DIR)/gdt.o $(KERNEL_DIR)/gdt_s.o $(KERNEL_DIR)/mem.o $(KERNEL_DIR)/idt_s.o $(KERNEL_DIR)/idt.o $(KERNEL_DIR)/pit.o \
	$(KERNEL_DIR)/ata.o $(KERNEL_DIR)/ext2.o $(KERNEL_DIR)/serial.o $(KERNEL_DIR)/shell.o $(KERNEL_DIR)/mouse.o $(KERNEL_DIR)/pmm.o \
	$(KERNEL_DIR)/vmm.o $(KERNEL_DIR)/memory.o

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


#
# Tools
#
mtools: $(TOOLS_DIR)/mkfs.labfs $(TOOLS_DIR)/mcopy.labfs $(TOOLS_DIR)/mdir.labfs $(TOOLS_DIR)/mdump.labfs

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
	mkdir -p $(BOOTLOADER_DIR)
	mkdir -p $(KERNEL_DIR)


#
# Run
#
run:
	$(QEMU) -hda $(BUILD_DIR)/labos-agarta.img -hdb ext2.img -serial stdio
