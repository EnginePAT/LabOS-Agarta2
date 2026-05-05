#
# Top-level Makefile
#

ASM      = nasm
CC       = i386-elf-gcc
LD       = i386-elf-ld
QEMU     = qemu-system-i386

E2MDIR   = e2mkdir
E2MFS    = mke2fs
E2CPY    = e2cp

BUILD_DIR      = build
BOOTLOADER_DIR = $(BUILD_DIR)/boot
KERNEL_DIR     = $(BUILD_DIR)/kernel
USER_DIR       = $(BUILD_DIR)/userspace
TOOLS_DIR      = tools

all: mtools bootloader kernel image

bootloader:
	$(MAKE) -C src/bootloader

kernel:
	$(MAKE) -C src/kernel

userland:
	$(MAKE) -C src/userspace

image: $(BUILD_DIR)/labos-agarta.img

$(BUILD_DIR)/labos-agarta.img: bootloader kernel userland | always
	@echo *** COPYING BOOTFS ***
	dd if=$(BOOTLOADER_DIR)/boot.bin of=$@ conv=notrunc
	./tools/mkfs.labfs $@
	./tools/mdir.labfs $@ boot /boot
	./tools/mcopy.labfs $@ config.cfg /config.cfg
	./tools/mcopy.labfs $@ $(BOOTLOADER_DIR)/stage2.bin /stage2.bin
	./tools/mcopy.labfs $@ $(KERNEL_DIR)/kernel.bin /boot/kernel.bin

	@echo *** CREATING FILESYSTEM ***
	dd if=/dev/zero of=$(BUILD_DIR)/ext2.img bs=1M count=64
	mkfs.ext2 -b 1024 $(BUILD_DIR)/ext2.img
	$(E2MDIR) $(BUILD_DIR)/ext2.img:/boot
	$(E2MDIR) $(BUILD_DIR)/ext2.img:/usr/system/
	$(E2MDIR) $(BUILD_DIR)/ext2.img:/bin
	$(E2MDIR) $(BUILD_DIR)/ext2.img:/lib
	$(E2MDIR) $(BUILD_DIR)/ext2.img:/home
	$(E2MDIR) $(BUILD_DIR)/ext2.img:/var
	$(E2MDIR) $(BUILD_DIR)/ext2.img:/tmp
	$(E2MDIR) $(BUILD_DIR)/ext2.img:/etc
	$(E2MDIR) $(BUILD_DIR)/ext2.img:/Frameworks
	$(E2MDIR) $(BUILD_DIR)/ext2.img:/Apps
	$(E2CPY) assets/test.txt $(BUILD_DIR)/ext2.img:/test.txt
	$(E2CPY) assets/LabOS-Mascot.bmp $(BUILD_DIR)/ext2.img:/etc/LabOS-Mascot.bmp
	$(E2CPY) $(USER_DIR)/userland.exe $(BUILD_DIR)/ext2.img:/usr/system/userland.exe

mtools: $(TOOLS_DIR)/mkfs.labfs $(TOOLS_DIR)/mcopy.labfs $(TOOLS_DIR)/mdir.labfs $(TOOLS_DIR)/mdump.labfs
	@echo *** BUILD LABFS MTOOLS ***
	@echo

$(TOOLS_DIR)/mkfs.labfs: $(TOOLS_DIR)/mkfs.labfs.c
	gcc $< -o $@

$(TOOLS_DIR)/mcopy.labfs: $(TOOLS_DIR)/mcopy.labfs.c
	gcc $< -o $@

$(TOOLS_DIR)/mdir.labfs: $(TOOLS_DIR)/mdir.labfs.c
	gcc $< -o $@

$(TOOLS_DIR)/mdump.labfs: $(TOOLS_DIR)/mdump.labfs.c
	gcc $< -o $@

clean:
	rm -rf $(BUILD_DIR)/*
	rm -f ext2.img
	rm -f $(TOOLS_DIR)/mcopy.labfs
	rm -f $(TOOLS_DIR)/mkfs.labfs
	rm -f $(TOOLS_DIR)/mdir.labfs
	rm -f $(TOOLS_DIR)/mdump.labfs

always:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BOOTLOADER_DIR)
	mkdir -p $(KERNEL_DIR)
	mkdir -p $(USER_DIR)

run:
	$(QEMU) \
	    -drive file=$(BUILD_DIR)/labos-agarta.img,format=raw,if=ide,index=0 \
	    -drive file=$(BUILD_DIR)/ext2.img,format=raw,if=ide,index=1 \
	    -serial stdio
