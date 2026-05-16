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
#include <kernel/core/vga/serial.h>
#include <kernel/core/vga/vga.h>
#include <kernel/core/keyboard.h>
#include <kernel/boot_info.h>

#include <kernel/core/mm/gdt.h>
#include <kernel/core/mm/idt.h>
#include <kernel/core/mm/pit.h>
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

#include <kernel/core/multitasking/process.h>
#include <kernel/core/multitasking/scheduler.h>

// Userspace
#include <userspace/userspace.h>
#include <userspace/core/mach4/loader.h>

static struct LFramebufferInfo* g_fb = 0;
uint32_t userland_entry;

void mach4_execute(uint32_t entry)
{
    uint32_t user_stack = USER_STACK_TOP;

    jump_usermode(entry, user_stack);
}

extern uint64_t ticks;

void task_a() {
    int size = 50;
    int dir = 1;
    uint64_t last_tick = 0;
    asm volatile("sti");

    while (1) {
        uint8_t* fb = (uint8_t*)g_fb->framebuffer;

        for (int y = 400; y < 500; y++)
            for (int x = 400; x < 500; x++) {
                uint32_t off = y * 3072 + x * 3;
                fb[off] = fb[off+1] = fb[off+2] = 0;
            }

        for (int y = 400; y < 400 + size; y++)
            for (int x = 400; x < 400 + size; x++) {
                uint32_t off = y * 3072 + x * 3;
                fb[off]   = 0xFF;
                fb[off+1] = 0x00;
                fb[off+2] = 0x00;
            }

        if (ticks - last_tick >= 10) {
            last_tick = ticks;
            size += dir;
            if (size >= 100 || size <= 1) dir = -dir;
        }
    }
}

void task_b() {
    int size = 25;
    int dir = 1;
    uint64_t last_tick = 0;
    asm volatile("sti");

    while (1) {
        uint8_t* fb = (uint8_t*)g_fb->framebuffer;

        for (int y = 300; y < 400; y++)
            for (int x = 300; x < 400; x++) {
                uint32_t off = y * 3072 + x * 3;
                fb[off] = fb[off+1] = fb[off+2] = 0;
            }

        for (int y = 300; y < 300 + size; y++)
            for (int x = 300; x < 300 + size; x++) {
                uint32_t off = y * 3072 + x * 3;
                fb[off]   = 0x00;
                fb[off+1] = 0xFF;
                fb[off+2] = 0x00;
            }

        if (ticks - last_tick >= 10) {
            last_tick = ticks;
            size += dir;
            if (size >= 100 || size <= 1) dir = -dir;
        }
    }
}

void draw_quadratic()
{
    // y = ax^2 + bx + c
    // We'll assume a = 1, b = 2, c = 3
    // Therefore we get:
    //  y = x^2 + 2x + 3
    uint8_t* fb = (uint8_t*)g_fb->framebuffer;
    int width  = g_fb->width;
    int height = g_fb->height;
    int pitch  = g_fb->pitch; // bytes per row

    int r = 50;
    int cx = 100;
    int cy = 500;

    asm volatile("sti");

    for (int y = -r; y <= r; y++) {
        for (int x = -r; x <= r; x++) {
            if (x*x + y*y <= r*r) {
                putpixel(cx + x, cy + y, 0xFFFFFFFF);
            }
        }
    }
}

void kernel_main(struct LBootInfo* boot_info, struct LFramebufferInfo* fb_info)
{
    // Copy boot info to safe kernel memory before vmm_init
    struct LBootInfo safe_boot_info   = *boot_info;
    struct LFramebufferInfo safe_fb_info = *fb_info;

    // Use safe copies from here on
    boot_info = &safe_boot_info;
    fb_info = &safe_fb_info;
    g_fb = &safe_fb_info;

    // Print a message
    setFbInfo(&safe_fb_info);
    vga_clear();
    vga_print("Hello, world!\n>_");

    initGdt();
    vga_print("[ OK ]: GDT is done.\n");
    initIdt();
    vga_print("[ OK ]: IDT is done.\n");
    initTimer();
    vga_print("[ OK ]: Timer is done.\n");

    e820_entry_t* mmap = (e820_entry_t*)boot_info->mmap_addr;
    serial_print("mmap_count=");
    serial_print_hex(safe_boot_info.mmap_count);
    serial_print(" mmap_addr=");
    serial_print_hex(safe_boot_info.mmap_addr);
    serial_print("\n");

    pmm_init(safe_boot_info.memory_size, mmap, safe_boot_info.mmap_count);
    vga_print("[ OK ]: PMM initialized successfully.\n");
    vmm_init(fb_info);
    vga_print("[ OK ]: Paging initialized successfully.\n");
    heap_init(0x200000, 0x100000);
    vga_print("[ OK ]: Heap is done.\n");
    pmm_reserve_region(0x200000, 0x100000);

    // Initialize the keyboard and eventually the mouse!
    keyboard_init();
    vga_print("[ OK ]: Keyboard initialized on IRQ1.\n");
    ext2_init();
    vga_print("[ OK ]: EXT2 initialized!\n");

    vfs_root = ext2_mount_root();
    vga_print("[ OK ]: EXT2 root mounted!\n");

    vga_print("[ STARTED ]: Opening /usr/system/userland.exe\n");
    vfs_node_t* n = vfs_open("/usr/system/userland.exe");
    uint8_t* program = kmalloc(n->size);

    // Map enough pages for the whole file
    vfs_read(n, 0, n->size, program);
    uint32_t code_size  = n->size;
    uint32_t code_pages = (code_size + PAGE_SIZE - 1) / PAGE_SIZE;

    // Veryify userland.exe is Mach4, and calculate the entry offset
    struct mach4_header_t* m4hdr = (struct mach4_header_t*)program;
    if (m4hdr->magic != MACH4_MAGIC) {
        vga_print("Invalid Mach4 Magic!\n");
        for(;;);
    }
    userland_entry = USER_CODE_BASE + m4hdr->entry_offset;
    vga_print("Mach4 Entry at: ");
    vga_print_hex(userland_entry);
    vga_print("\n");

    for (uint32_t i = 0; i < code_pages; i++) {
        uint32_t virt   = USER_CODE_BASE + i * PAGE_SIZE;
        uint32_t phys   = pmm_alloc();
        uint32_t offset = i * PAGE_SIZE;
        uint32_t chunk  = (offset + PAGE_SIZE > n->size) ? n->size - offset : PAGE_SIZE;

        kmemcpy((void*)phys, program + offset, chunk);
        vmm_map_page(current_dir, virt, phys, PAGE_USER | PAGE_WRITEABLE);
    }
    current_dir[PAGE_DIR_INDEX(USER_CODE_BASE)] |= PAGE_USER;
    vga_print("[ DONE ]: Finished loading userland.exe.\n");


    // Setup the scheduler and multitasking
    process_init();
    // process_t* uproc = process_create(entry, current_dir);
    process_t* ta = process_create((uint32_t)task_a, current_dir);
    process_t* tb = process_create((uint32_t)task_b, current_dir);
    process_create((uint32_t)userspace_init, current_dir);

    // Start task_a as the first program in the scheduler
    process_t* p = ta;
    process_t* prev = current_process;
    current_process = p;
    prev->state = P_DEAD;
    p->state = P_RUNNING;

    context_switch(&prev->esp, p->esp, (uint32_t)p->page_dir);


    vga_print("Jumping to usermode.\n");
    // userspace_init(entry);

    while (1);
}
