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
all: $(BUILD_DIR)/labos-agarta.img

$(BUILD_DIR)/labos-agarta.img: $(BUILD_DIR)/boot.bin $(BUILD_DIR)/stage2.bin $(BUILD_DIR)/zeroes.bin
	./tools/mkfs.labfs $@
	dd if=$(BUILD_DIR)/boot.bin of=$@ conv=notrunc


#
# Stage 1 Bootloader
#
$(BUILD_DIR)/boot.bin: $(SRC_DIR)/bootloader/boot.asm
	$(ASM) $< -f bin -o $@

$(BUILD_DIR)/zeroes.bin: $(SRC_DIR)/bootloader/zeroes.asm
	$(ASM) $< -f bin -o $@


#
# Stage 2 Bootloader
#
$(BUILD_DIR)/stage2.bin: $(SRC_DIR)/bootloader/stage2/stage2_entry.asm $(SRC_DIR)/bootloader/stage2/stage2.c
	$(CC) $(CFLAGS) $(SRC_DIR)/bootloader/stage2/stage2.c -o $(BUILD_DIR)/stage2.o
	$(ASM) $(SRC_DIR)/bootloader/stage2/stage2_entry.asm -f elf32 -o $(BUILD_DIR)/stage2_entry.o
	$(LD) -T $(SRC_DIR)/linker.ld $(BUILD_DIR)/stage2_entry.o $(BUILD_DIR)/stage2.o -o $@ --oformat binary


#
# Tools
#
mtools: $(TOOLS_DIR)/mkfs.labfs

$(TOOLS_DIR)/mkfs.labfs: $(TOOLS_DIR)/mkfs.labfs.c
	gcc $< -o $@


#
# Clean
#
clean:
	rm -rf $(BUILD_DIR)/*


#
# Run
#
run:
	$(QEMU) -hda $(BUILD_DIR)/labos-agarta.img -drive file=disk.img -serial stdio
