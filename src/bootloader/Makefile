#
# * LabOS Agarta
# * Copyright (c) 2026 EnginePAT
# *
# * Licensed under the Apache License, Version 2.0 (the "License");
# * you may not use this file except in compliance with the License.
# * You may obtain a copy of the License at
# *
# *     http://www.apache.org/licenses/LICENSE-2.0
# *
# * Unless required by applicable law or agreed to in writing, software
# * distributed under the License is distributed on an "AS IS" BASIS,
# * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#

#
# Bootloader Makefile
#

ASM  = nasm
CC   = i386-elf-gcc
LD   = i386-elf-ld

SRC_DIR        = ..
BUILD_DIR      = ../../build
BOOTLOADER_DIR = $(BUILD_DIR)/boot

CFLAGS = -ffreestanding -nostdlib -m32 -I../../include/ -g -c

all: stage1 stage2

stage1: $(BOOTLOADER_DIR)/boot.bin
	@echo *** BUILD STAGE 1 BOOTLOADER ***

$(BOOTLOADER_DIR)/boot.bin: $(SRC_DIR)/bootloader/boot.asm | always
	$(ASM) $< -f bin -o $@

$(BOOTLOADER_DIR)/zeroes.bin: $(SRC_DIR)/bootloader/zeroes.asm | always
	$(ASM) $< -f bin -o $@

stage2: $(BOOTLOADER_DIR)/stage2.bin
	@echo *** BUILD STAGE 2 BOOTLOADER ***

STAGE2_OBJS = \
	$(BOOTLOADER_DIR)/stage2_entry.o \
	$(BOOTLOADER_DIR)/stage2.o \
	$(BOOTLOADER_DIR)/util.o \
	$(BOOTLOADER_DIR)/ata.o \
	$(BOOTLOADER_DIR)/serial.o

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

always:
	mkdir -p $(BOOTLOADER_DIR)

clean:
	rm -f $(BOOTLOADER_DIR)/*.bin
	rm -f $(BOOTLOADER_DIR)/*.o
