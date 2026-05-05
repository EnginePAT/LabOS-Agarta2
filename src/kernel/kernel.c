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

// Userspace
#include <userspace/userspace.h>

extern void jump_usermode();

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

    // // Initialize the keyboard and eventually the mouse!
    keyboard_init();

    // float x = 2 / 0;            // We can't do this without getting an infinite result - should trigger a fault

    ext2_init();

    vfs_root = ext2_mount_root();

    vfs_node_t* n = vfs_open("/usr/system/userland.exe");
    uint8_t* program = kmalloc(n->size + 1);
    vfs_read(n, 0, n->size, program);
    struct mach4_header_t* header = (struct mach4_header_t*)program;
    if (header->magic != MACH4_MAGIC)
    {
        vga_print("Invalid Mach4 executeable!");
        while (1);
    }

    uint32_t code_size = n->size - header->entry_offset;
    uint32_t code_pages = (code_size + PAGE_SIZE - 1) / PAGE_SIZE;

    for (uint32_t i = 0; i < code_pages; i++) {
        uint32_t virt = USER_CODE_BASE + i * PAGE_SIZE;
        uint32_t phys = pmm_alloc();
        vmm_map_page(current_dir, virt, phys, PAGE_USER | PAGE_WRITEABLE);

        // copy this page worth of data
        uint32_t offset = header->entry_offset + i * PAGE_SIZE;
        uint32_t chunk  = PAGE_SIZE;
        if (offset + chunk > n->size)
            chunk = n->size - offset;

        memcpy((void*)virt, program + offset, chunk);
    }

    // If that succeeds, we can try to execute the executable
    // uint32_t entry = (uint32_t)program + header->entry_offset;
    // void (*entry_fn)(void) = (void(*)(void))entry;
    // entry_fn();
    serial_print("Checking user code page: ");
    serial_print_hex(*(uint32_t*)USER_CODE_BASE);
    serial_print("\n");
    
    uint32_t entry = USER_CODE_BASE;
    mach4_execute(entry);

    userspace_init();

    while (1);
}
