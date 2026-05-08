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
#include "userspace/core/mach4/loader.h"
#include <kernel/core/vga/serial.h>
#include <kernel/core/vga/vga.h>
#include <kernel/core/keyboard.h>
#include <kernel/boot_info.h>

#include <kernel/core/mm/gdt.h>
#include <kernel/core/mm/idt.h>
#include <kernel/core/mm/pic.h>
#include <kernel/core/mm/pmm.h>
#include <kernel/core/mm/vmm.h>
#include <kernel/core/mm/memory.h>

#include <kernel/fs/ext2.h>
#include <kernel/fs/ext2_vfs.h>
#include <kernel/fs/vfs.h>
#include <kernel/core/shell.h>
#include <kernel/core/mouse.h>
#include <stdint.h>
#include <util/mem.h>
#include <userspace/core/mach4/loader.h>

// Userspace
#include <userspace/userspace.h>

void mach4_execute(uint32_t entry)
{
    uint32_t user_stack = USER_STACK_TOP;

    jump_usermode(entry, user_stack);
}

void kernel_main(struct LBootInfo* boot_info, struct LFramebufferInfo* fb_info)
{
    // Copy boot info to safe kernel memory before vmm_init
    struct LBootInfo safe_boot_info   = *boot_info;
    struct LFramebufferInfo safe_fb_info = *fb_info;

    // Use safe copies from here on
    boot_info = &safe_boot_info;
    fb_info = &safe_fb_info;

    // Print a message
    setFbInfo(&safe_fb_info);
    vga_clear();
    vga_print("Hello, world!\n>_");

    initGdt();
    initIdt();
    initTimer();

    e820_entry_t* mmap = (e820_entry_t*)boot_info->mmap_addr;
    serial_print("mmap_count=");
    serial_print_hex(safe_boot_info.mmap_count);
    serial_print(" mmap_addr=");
    serial_print_hex(safe_boot_info.mmap_addr);
    serial_print("\n");

    pmm_init(safe_boot_info.memory_size, mmap, safe_boot_info.mmap_count);
    vmm_init(fb_info);
    heap_init(0x200000, 0x100000);
    pmm_reserve_region(0x200000, 0x100000);

    // Initialize the keyboard and eventually the mouse!
    keyboard_init();
    ext2_init();

    vfs_root = ext2_mount_root();

    vfs_node_t* n = vfs_open("/usr/system/userland.exe");
    uint8_t* program = kmalloc(n->size);

    serial_print("n->size = ");
    serial_print_hex(n->size);
    serial_print("\n");

    // Map enough pages for the whole file
    vfs_read(n, 0, n->size, program);
    uint32_t code_size  = n->size;
    uint32_t code_pages = (code_size + PAGE_SIZE - 1) / PAGE_SIZE;
    
    serial_print("userland size=");
    serial_print_hex(n->size);
    serial_print("\n");

    for (uint32_t i = 0; i < code_pages; i++) {
        uint32_t virt   = USER_CODE_BASE + i * PAGE_SIZE;
        uint32_t phys   = pmm_alloc();
        uint32_t offset = i * PAGE_SIZE;
        uint32_t chunk  = (offset + PAGE_SIZE > n->size) ? n->size - offset : PAGE_SIZE;

        kmemcpy((void*)phys, program + offset, chunk);
        vmm_map_page(current_dir, virt, phys, PAGE_USER | PAGE_WRITEABLE);
    }
    current_dir[PAGE_DIR_INDEX(USER_CODE_BASE)] |= PAGE_USER;
    // asm volatile("mov %%cr3, %%eax; mov %%eax, %%cr3" ::: "eax");

    // After the mapping loop
    uint32_t pte = ((uint32_t*)PAGE_FRAME(current_dir[PAGE_DIR_INDEX(USER_CODE_BASE)]))[PAGE_TABLE_INDEX(USER_CODE_BASE)];
    uint32_t phys_base = PAGE_FRAME(pte);
    serial_print("string at phys+0x660: ");
    serial_print((char*)(phys_base + 0x660));
    serial_print("\n");

    serial_print("First dword at USER_CODE_BASE: ");
    serial_print_hex(*(uint32_t*)USER_CODE_BASE);
    serial_print("\n");

    userspace_init();

    while (1);
}
