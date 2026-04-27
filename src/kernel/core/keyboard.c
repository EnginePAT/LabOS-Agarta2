#include <kernel/core/vga/vga.h>
#include <util/util.h>
#include <kernel/core/keyboard.h>
#include <kernel/mm/idt.h>
#include <util/util.h>
#include <stdint.h>

static int shift = 0;
static int caps = 0;

static void keyboard_irq(struct InterruptRegisters* regs);

void keyboard_init()
{
    irq_install_handler(1, keyboard_irq);
}

static void keyboard_irq(struct InterruptRegisters* regs)
{
    uint8_t scancode = inb(0x60);       // Get the scancode from the PS/2 keyboard port

    // Handle key release
    if (scancode & 0x80)
    {
        uint8_t released = scancode & 0x7F;
        if (released == 0x2A || released == 0x36) shift = 0;        // Shift released
        return;
    }

    // Shift pressed
    if (scancode == 0x2A || scancode == 0x36)
    {
        shift = 1;
        return;
    }

    // Caps lock toggle
    if (scancode == 0x3A)
    {
        caps = !caps;
        return;
    }

    char c = shift ? keymap_shift[scancode] : keymap[scancode];

    // Apply caps lock to letters only
    if (caps && c >= 'a' && c <= 'z') c -= 32;
    if (caps && c >= 'A' && c <= 'Z') c += 32;

    if (c)
    {
        vga_putchar(c);
    }
}
