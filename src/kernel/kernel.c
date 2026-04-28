#include <kernel/core/vga/serial.h>
#include <kernel/core/vga/vga.h>
#include <kernel/core/keyboard.h>
#include <kernel/boot_info.h>

#include <kernel/mm/gdt.h>
#include <kernel/mm/idt.h>
#include <kernel/mm/pic.h>

#include <kernel/fs/ext2.h>
#include <kernel/core/shell.h>
#include <kernel/core/mouse.h>
#include <stdint.h>

void kernel_main(struct LBootInfo* boot_info, struct LFramebufferInfo* fb_info)
{
    // Print a message
    vga_clear();
    setFbInfo(fb_info);
    vga_print("Hello, world!\n>_");

    initGdt();
    initIdt();
    initTimer();

    // Initialize the keyboard and eventually the mouse!
    keyboard_init();
    mouse_init();

    // float x = 2 / 0;            // We can't do this without getting an infinite result - should trigger a fault

    ext2_init();

    // Read the root inode and get its data
    uint32_t inode_num = ext2_find_entry(2, "etc");
    if (inode_num != 0) {
        uint32_t bmp_inode_num = ext2_find_entry(inode_num, "LabOS-Mascot.bmp");
        read_entries(inode_num);

        if (bmp_inode_num != 0)
        {
            char test[1024];
            ext2_read_file(bmp_inode_num, test);
            vga_print(test);
        } else {
            serial_print("Invalid read for bmp image.\n");
        }
    } else {
        serial_print("Invalid read.\n");
    }

    // Run the shell
    shell_main();

    while (1);
}
