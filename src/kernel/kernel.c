#include <kernel/core/vga/serial.h>
#include <kernel/core/vga/vga.h>
#include <kernel/core/keyboard.h>
#include <kernel/boot_info.h>

#include <kernel/mm/gdt.h>
#include <kernel/mm/idt.h>
#include <kernel/mm/pic.h>
#include <kernel/mm/pmm.h>
#include <kernel/mm/vmm.h>

#include <kernel/fs/ext2.h>
#include <kernel/core/shell.h>
#include <kernel/core/mouse.h>
#include <util/mem.h>
// #include <stdint.h>

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
    vga_print("Testing...");

    // // Initialize the keyboard and eventually the mouse!
    keyboard_init();

    // float x = 2 / 0;            // We can't do this without getting an infinite result - should trigger a fault

    ext2_init();

    // Read the root inode and get its data
    // uint32_t inode_num = ext2_find_entry(2, "etc");
    // if (inode_num != 0) {
    //     uint32_t bmp_inode_num = ext2_find_entry(inode_num, "LabOS-Mascot.bmp");
    //     read_entries(inode_num);

    //     if (bmp_inode_num != 0)
    //     {
    //         char test[1024];
    //         ext2_read_file(bmp_inode_num, test);
    //         vga_print(test);
    //     } else {
    //         serial_print("Invalid read for bmp image.\n");
    //     }
    // } else {
    //     serial_print("Invalid read.\n");
    // }

    while (1);
}
