#include <kernel/core/mouse.h>
#include <util/util.h>
#include <kernel/core/mm/idt.h>
#include <kernel/core/vga/vga.h>

void mouse_irq(struct InterruptRegisters* regs);

static uint8_t packet[3];
static uint8_t packet_index = 0;

static int mouse_x = 400;
static int mouse_y = 300;
static int old_x = 0;
static int old_y = 0;
static uint32_t cursor_bg[3][3];

void mouse_init()
{
    outb(0x64, 0xA8);       			// Enable the auxiliary device

    // Tell the controller to send mouse IRQs
    outb(0x64, 0x20);				// read command byte
    uint8_t status = inb(0x60);
	status |= 2;								// enable IRQ12
    outb(0x64, 0x60);
    outb(0x60, status);

	// Reset mouse
	mouse_write(0xF6);	mouse_read();	// Default settings
	mouse_write(0xF4);	mouse_read();	// Enable data reporting

    irq_install_handler(12, mouse_irq);
}

void mouse_irq(struct InterruptRegisters* regs)
{
	uint8_t data = inb(0x60);

    packet[packet_index++] = data;

    if (packet_index < 3)
        return;

    packet_index = 0;

    int dx = packet[1];
    int dy = packet[2];

    // Sign extend
    if (packet[0] & 0x10) dx |= 0xFFFFFF00;
    if (packet[0] & 0x20) dy |= 0xFFFFFF00;

    mouse_x += dx;
    mouse_y -= dy; // Y is inverted

    // Clamp
    if (mouse_x < 0) mouse_x = 0;
    if (mouse_y < 0) mouse_y = 0;
    if (mouse_x > 1023) mouse_x = 1023;
    if (mouse_y > 767) mouse_y = 767;

	// Draw the mouse
	mouse_draw(mouse_x, mouse_y);
}

static void mouse_wait(uint8_t type)
{
    uint32_t timeout = 100000;
    if (type == 0) {
        while (timeout--) {
            if (inb(0x64) & 1) return;
        }
    } else {
        while (timeout--) {
            if (!(inb(0x64) & 2)) return;
        }
    }
}

static void mouse_write(uint8_t value)
{
    mouse_wait(1);
    outb(0x64, 0xD4);
    mouse_wait(1);
    outb(0x60, value);
}

static uint8_t mouse_read()
{
    mouse_wait(0);
    return inb(0x60);
}

void mouse_draw(int x, int y)
{
    // Restore old background
    for (int dy = 0; dy < 3; dy++)
        for (int dx = 0; dx < 3; dx++)
            putpixel(old_x + dx, old_y + dy, cursor_bg[dy][dx]);

    // Save new background
    for (int dy = 0; dy < 3; dy++)
        for (int dx = 0; dx < 3; dx++)
            cursor_bg[dy][dx] = getpixel(x + dx, y + dy);

    // Draw cursor
    putpixel(x, y, 0xFFFFFF);
    putpixel(x+1, y, 0xFFFFFF);
    putpixel(x, y+1, 0xFFFFFF);

    old_x = x;
    old_y = y;
}
