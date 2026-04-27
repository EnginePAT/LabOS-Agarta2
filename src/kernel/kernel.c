#include <stdint.h>
#include <kernel/core/vga/vga.h>
#include <kernel/core/keyboard.h>

#include <kernel/mm/gdt.h>
#include <kernel/mm/idt.h>
#include <kernel/mm/pic.h>

#include <kernel/fs/ext2.h>

void kernel_main(unsigned int magic, uint8_t* addr)
{
    // Print a message
    vga_clear();
    vga_print("Hello, world!\n>_");

    initGdt();
    initIdt();
    initTimer();

    // float x = 2 / 0;            // We can't do this without getting an infinite result - should trigger a fault

    ext2_init();

    while (1)
    {
        char c = keyboard_poll();
        if (c) vga_putchar(c);
    }

    while (1);
}
