#include <stdint.h>
#include <kernel/core/vga/vga.h>
#include <kernel/core/keyboard.h>

void kernel_main(unsigned int magic, uint8_t* addr)
{
    // Print a message
    vga_clear();
    vga_print("Hello, world!\n>_");

    while (1)
    {
        char c = keyboard_poll();
        if (c) vga_putchar(c);
    }

    while (1);
}
