#include <kernel/core/vga/vga.h>
#include <stdint.h>

uint16_t* video = (uint16_t*)0xb8000;
static int cursor_x = 0;
static int cursor_y = 0;

void vga_putchar(char c)
{
    if (c == '\n')
    {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\r')
    {
        cursor_x = 0;
    } else if (c == '\b')
    {
        if (cursor_x > 0)
        {
            cursor_x--;
        } else if (cursor_y > 0)
        {
            cursor_y--;
            cursor_x = VGA_WIDTH - 1;
        }
        video[cursor_y * VGA_WIDTH + cursor_x] = (0x0f << 8) | ' ';
    } else
    {
        video[cursor_y * VGA_WIDTH + cursor_x] = (0x0f << 8) | c;
        cursor_x++;
        if (cursor_x >= VGA_WIDTH)
        {
            cursor_x = 0;
            cursor_y++;
        }
    }

    // Scroll if needed
    if (cursor_y >= VGA_HEIGHT) {
        for (int y = 1; y < VGA_HEIGHT; y++)
        {
            for (int x = 0; x < VGA_WIDTH; x++)
            {
                video[(y-1) * VGA_WIDTH + x] = video[y * VGA_WIDTH + x];
            }
        }
        for (int x = 0; x < VGA_WIDTH; x++)
        {
            video[(VGA_HEIGHT-1) * VGA_WIDTH + x] = (0x0f << 8) | ' ';
        }
        cursor_y = VGA_HEIGHT - 1;
    }
}

void vga_print(String s)
{
    while (*s) vga_putchar(*s++);
}

void vga_clear(void) {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
    {
        video[i] = (0x0f << 8) | ' ';
    }
    cursor_x = cursor_y = 0;
}
