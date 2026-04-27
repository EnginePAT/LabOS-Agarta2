#include "kernel/core/vga/serial.h"
#include <kernel/core/vga/vga.h>
#include <kernel/core/keyboard.h>
#include <kernel/boot_info.h>

#include <kernel/mm/gdt.h>
#include <kernel/mm/idt.h>
#include <kernel/mm/pic.h>

#include <kernel/fs/ext2.h>
#include <kernel/core/shell.h>
#include <kernel/core/mouse.h>

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
    serial_print("Bootloader Magic: ");
    serial_print_hex(boot_info->magic);
    serial_print("\n");

    // Run the shell
    // shell_main();

    while (1);
}
