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

ASM     = nasm
CC      = i386-elf-gcc
LD      = i386-elf-ld
OBJCOPY = i386-elf-objcopy

CFLAGS  = -ffreestanding -nostdlib -m32 -I../../include/ -g -c

USER_DIR = ../../build/userspace

all: $(USER_DIR)/userland.exe

$(USER_DIR)/userland.exe: userland_entry.asm userland.c mach4.ld
	$(CC) $(CFLAGS) userland.c -o $(USER_DIR)/userland.o
	$(CC) $(CFLAGS) core/syscall.c -o $(USER_DIR)/syscall.o

	$(ASM) -f elf32 userland_entry.asm -o $(USER_DIR)/mach4_entry.o
	$(LD) -T mach4.ld -o $(USER_DIR)/userland.elf $(USER_DIR)/mach4_entry.o $(USER_DIR)/userland.o $(USER_DIR)/syscall.o
	$(OBJCOPY) -O binary $(USER_DIR)/userland.elf $(USER_DIR)/userland.exe
