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
TOOLS_DIR=tools


#
# Image
#
all: mtools $(BUILD_DIR)/labos-agarta.img

$(BUILD_DIR)/labos-agarta.img: stage1 stage2 kernel
	dd if=$(BUILD_DIR)/boot.bin of=$@ conv=notrunc
	./tools/mkfs.labfs $@
	./tools/mdir.labfs $@ boot /boot
	./tools/mcopy.labfs $@ config.cfg /config.cfg
	./tools/mcopy.labfs $@ $(BUILD_DIR)/stage2.bin /stage2.bin
	./tools/mcopy.labfs $@ $(BUILD_DIR)/kernel.bin /boot/kernel.bin


#
# Stage 1 Bootloader
#
stage1: $(BUILD_DIR)/boot.bin

$(BUILD_DIR)/boot.bin: $(SRC_DIR)/bootloader/boot.asm
	$(ASM) $< -f bin -o $@

$(BUILD_DIR)/zeroes.bin: $(SRC_DIR)/bootloader/zeroes.asm
	$(ASM) $< -f bin -o $@


#
# Stage 2 Bootloader
#
stage2: $(BUILD_DIR)/stage2.bin

$(BUILD_DIR)/stage2.bin: $(SRC_DIR)/bootloader/stage2/stage2_entry.asm $(SRC_DIR)/bootloader/stage2/stage2.c
	$(CC) $(CFLAGS) $(SRC_DIR)/bootloader/stage2/stage2.c -o $(BUILD_DIR)/stage2.o
	$(ASM) $(SRC_DIR)/bootloader/stage2/stage2_entry.asm -f elf32 -o $(BUILD_DIR)/stage2_entry.o
	$(LD) -T $(SRC_DIR)/linker.ld $(BUILD_DIR)/stage2_entry.o $(BUILD_DIR)/stage2.o -o $@ --oformat binary


#
# Kernel
#
kernel: $(BUILD_DIR)/kernel.bin

$(BUILD_DIR)/kernel.bin: $(BUILD_DIR)/kernel_entry.o $(BUILD_DIR)/kernel.o
	$(LD) -T $(SRC_DIR)/linker.ld $^ -o $@ --oformat binary

$(BUILD_DIR)/kernel_entry.o: $(SRC_DIR)/kernel/kernel_entry.asm
	$(ASM) $< -f elf32 -o $@

$(BUILD_DIR)/kernel.o: $(SRC_DIR)/kernel/kernel.c
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
	rm -rf $(TOOLS_DIR)/mcopy.labfs
	rm -rf $(TOOLS_DIR)/mkfs.labfs
	rm -rf $(TOOLS_DIR)/mdir.labfs
	rm -rf $(TOOLS_DIR)/mdump.labfs


#
# Run
#
run:
	$(QEMU) -hda $(BUILD_DIR)/labos-agarta.img -drive file=disk.img -serial stdio
