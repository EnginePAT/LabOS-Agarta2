#include "kernel/core/vga/serial.h"
#include <kernel/core/vga/vga.h>
#include <kernel/core/keyboard.h>
#include <kernel/boot_info.h>

#include <kernel/mm/gdt.h>
#include <kernel/mm/idt.h>
#include <kernel/mm/pic.h>

#include <kernel/fs/ext2.h>
#include <stdint.h>

static void putpixel(struct LFramebufferInfo* fb, int x, int y, uint32_t color)
{
    uint8_t* addr = (uint8_t*)(fb->framebuffer + y * fb->pitch + x * (fb->bpp / 8));

    // VESA uses BGR for 24/32-bit modes
    addr[0] = (color >> 8) & 0xFF;      // Blue
    addr[1] = (color >> 8) & 0xFF;      // Green
    addr[2] = (color >> 8) & 0xFF;      // Red

    if (fb->bpp == 32) addr[3] = 0xFF;  // Alpha (ignored for our case, just here if we use 32-bit mode)
}

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

    // float x = 2 / 0;            // We can't do this without getting an infinite result - should trigger a fault

    ext2_init();
    serial_print("Bootloader Magic: ");
    serial_print_hex(boot_info->magic);
    serial_print("\n");

    fb_print("Hello, world!", 200, 200, 0xFFFFFFFF, 0x00000000);

    while (1);
}
