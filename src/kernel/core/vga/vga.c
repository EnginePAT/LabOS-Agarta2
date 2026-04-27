#include "kernel/boot_info.h"
#include <kernel/core/vga/vga.h>
#include <stdint.h>
#include <kernel/core/vga/vga_8x16.h>

uint16_t* video = (uint16_t*)0xb8000;
static int cursor_x = 0;
static int cursor_y = 0;
struct LFramebufferInfo* _fb_info;

void setFbInfo(struct LFramebufferInfo* info)
{
    _fb_info = info;
}

void draw_glyph(char c, int x, int y, uint32_t fg, uint32_t bg)
{
    uint8_t* fb = (uint8_t*)_fb_info->framebuffer;
    const uint8_t* glyph = vga_font[(unsigned char)c];

    for (int row = 0; row < 16; row++)
    {
        uint8_t bits = glyph[row];
        for (int col = 0; col < 8; col++)
        {
            uint32_t color = (bits >> (7 - col)) & 1 ? fg : bg;
            uint32_t offset = (y + row) * _fb_info->pitch + (x + col) * (_fb_info->bpp / 8);

            fb[offset + 0] = color & 0xFF;          // B
            fb[offset + 1] = (color >> 8) & 0xFF;   // G
            fb[offset + 2] = (color >> 16) & 0xFF;  // R
        }
    }
}

void vga_putchar(char c)
{
    if (c == '\n')
    {
        cursor_x = 0;
        cursor_y += FONT_HEIGHT;
    } else if (c == '\r')
    {
        cursor_x = 0;
    } else if (c == '\b')
    {
        if (cursor_x > 0)
        {
            cursor_x -= FONT_WIDTH;
        } else if (cursor_y > 0)
        {
            cursor_y -= FONT_HEIGHT;
            cursor_x = _fb_info->width - 1;
        }
        draw_glyph(' ', cursor_x, cursor_y, 0xFFFFFFFF, 0x00000000);
    } else
    {
        draw_glyph(c, cursor_x, cursor_y, 0xFFFFFFFF, 0x00000000);
        cursor_x += FONT_WIDTH;
        if (cursor_x >= _fb_info->width)
        {
            cursor_x = 0;
            cursor_y += FONT_HEIGHT;
        }
    }

    // Scroll if needed
    if (cursor_y >= _fb_info->height) {
        for (int y = 1; y < _fb_info->height; y++)
        {
            for (int x = 0; x < _fb_info->width; x++)
            {
                video[(y-1) * _fb_info->width + x] = video[y * _fb_info->width + x];
            }
        }
        for (int x = 0; x < _fb_info->width; x++)
        {
            video[(_fb_info->height - 1) * _fb_info->width + x] = (0x0f << 8) | ' ';
        }
        cursor_y = _fb_info->height - 1;
    }
}

void vga_print(String s)
{
    while (*s) vga_putchar(*s++);
}

void fb_print(const char* s, int x, int y, uint32_t fg, uint32_t bg)
{
    while (*s)
    {
        draw_glyph(*s, x, y, fg, bg);
        x += 8;
        s++;
    }
}

void vga_clear(void) {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
    {
        video[i] = (0x0f << 8) | ' ';
    }
    cursor_x = cursor_y = 0;
}
