#include <util/util.h>
#include <kernel/core/keyboard.h>
#include <util/util.h>
#include <stdint.h>

static int shift = 0;
static int caps = 0;

char keyboard_poll()
{
    // Wait for a keypress
    while (!(inb(0x64) & 0x01));

    uint8_t scancode = inb(0x60);

    // Key release (high bit set)
    if (scancode & 0x80)
    {
        uint8_t released = scancode & 0x7F;
        if (released == 0x2A || released == 0x36) shift = 0;        // Shift released
        return 0;
    }

    // Shift pressed
    if (scancode == 0x2A || scancode == 0x36)
    {
        shift = 1;
        return 0;
    }

    // Caps lock toggle
    if (scancode == 0x3A)
    {
        caps = !caps;
        return 0;
    }

    char c = shift ? keymap_shift[scancode] : keymap[scancode];

    // Apply caps lock to letters only
    if (caps && c >= 'a' && c <= 'z') c -= 32;
    if (caps && c >= 'A' && c <= 'Z') c += 32;
    return c;
}
