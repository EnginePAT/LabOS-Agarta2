/*
 * LabOS Agarta
 * Copyright (c) 2026 EnginePAT
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */
#include <kernel/core/mm/vmm.h>
#include <kernel/core/vga/vga.h>
#include <stdint.h>
#include <userspace/userspace.h>
#include <kernel/core/mm/gdt.h>

void userspace_init(uint32_t entry)
{
    // Set kernel stack in TSS
    extern struct tss_entry_struct tss_entry;
    tss_entry.esp0 = 0x90000;             // Kernel stack
    tss_entry.ss0 = 0x10;                 // Kernel data segment

    vga_print("Welcome to Userland!\n");

    // Call our assembly function which jumps to Ring 3
    jump_usermode(entry, USER_STACK_TOP - 4);      // Hardcoded USER_CODE_BASE, not nice :(
}
