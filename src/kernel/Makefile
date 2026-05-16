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
# Kernel Makefile
#

ASM  = nasm
CC   = i386-elf-gcc
LD   = i386-elf-ld

SRC_DIR        = ..
BUILD_DIR      = ../../build
KERNEL_DIR     = $(BUILD_DIR)/kernel
USER_DIR       = $(BUILD_DIR)/userspace

CFLAGS = -ffreestanding -nostdlib -m32 -I../../include/ -g -c

all: $(KERNEL_DIR)/kernel.bin
	@echo *** BUILD KERNEL ***

KERNEL_OBJS = \
    $(KERNEL_DIR)/kernel_entry.o \
    $(KERNEL_DIR)/kernel.o \
    $(KERNEL_DIR)/util.o \
    $(KERNEL_DIR)/vga.o \
    $(KERNEL_DIR)/keyboard.o \
    $(KERNEL_DIR)/gdt.o \
    $(KERNEL_DIR)/gdt_s.o \
    $(KERNEL_DIR)/mem.o \
    $(KERNEL_DIR)/idt_s.o \
    $(KERNEL_DIR)/idt.o \
    $(KERNEL_DIR)/pit.o \
    $(KERNEL_DIR)/ata.o \
    $(KERNEL_DIR)/ext2.o \
    $(KERNEL_DIR)/serial.o \
    $(KERNEL_DIR)/shell.o \
    $(KERNEL_DIR)/mouse.o \
    $(KERNEL_DIR)/pmm.o \
    $(KERNEL_DIR)/vmm.o \
    $(KERNEL_DIR)/memory.o \
    $(USER_DIR)/userspace.o \
    $(USER_DIR)/userspace_s.o \
    $(USER_DIR)/syscall_handler.o \
    $(USER_DIR)/syscall_handler_s.o \
    $(USER_DIR)/syscall.o \
    $(USER_DIR)/handlers.o \
	$(KERNEL_DIR)/vfa.o \
	$(KERNEL_DIR)/ext2_vfs.o \
	$(KERNEL_DIR)/scheduler.o \
	$(KERNEL_DIR)/context_switch.o \
	$(KERNEL_DIR)/process.o

$(KERNEL_DIR)/kernel.bin: $(KERNEL_DIR)/kernel.elf
	$(LD) -T $(SRC_DIR)/kernel.ld $^ -o $@ --oformat binary

$(KERNEL_DIR)/kernel.elf: $(KERNEL_OBJS) | always
	$(LD) -T $(SRC_DIR)/kernel.ld $^ -o $@

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

$(KERNEL_DIR)/vfa.o: $(SRC_DIR)/kernel/fs/vfs.c | always
	$(CC) $(CFLAGS) $< -o $@

$(KERNEL_DIR)/ext2_vfs.o: $(SRC_DIR)/kernel/fs/ext2_vfs.c | always
	$(CC) $(CFLAGS) $< -o $@

$(KERNEL_DIR)/shell.o: $(SRC_DIR)/kernel/core/shell.c | always
	$(CC) $(CFLAGS) $< -o $@

$(KERNEL_DIR)/pmm.o: $(SRC_DIR)/kernel/core/mm/pmm.c | always
	$(CC) $(CFLAGS) $< -o $@

$(KERNEL_DIR)/vmm.o: $(SRC_DIR)/kernel/core/mm/vmm.c | always
	$(CC) $(CFLAGS) $< -o $@

$(KERNEL_DIR)/memory.o: $(SRC_DIR)/kernel/core/mm/memory.c | always
	$(CC) $(CFLAGS) $< -o $@

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

$(USER_DIR)/handlers.o: $(SRC_DIR)/userspace/core/handlers/handlers.c | always
	$(CC) $(CFLAGS) $< -o $@

$(KERNEL_DIR)/scheduler.o: $(SRC_DIR)/kernel/core/multitasking/scheduler.c | always
	$(CC) $(CFLAGS) $< -o $@

$(KERNEL_DIR)/process.o: $(SRC_DIR)/kernel/core/multitasking/process.c | always
	$(CC) $(CFLAGS) $< -o $@

$(KERNEL_DIR)/context_switch.o: $(SRC_DIR)/kernel/core/multitasking/context_switch.asm | always
	$(ASM) $< -f elf32 -o $@

always:
	mkdir -p $(KERNEL_DIR)
	mkdir -p $(USER_DIR)

clean:
	rm -f $(KERNEL_DIR)/*.o
	rm -f $(KERNEL_DIR)/kernel.bin
	rm -f $(USER_DIR)/*.o
