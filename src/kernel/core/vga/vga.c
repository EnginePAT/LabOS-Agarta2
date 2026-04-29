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

void putpixel(int x, int y, uint32_t color)
{
    uint32_t* fb = (uint32_t*)_fb_info->framebuffer;
    uint32_t pitch_pixels = _fb_info->pitch / 4;
    fb[y * pitch_pixels + x] = color;
}

uint32_t getpixel(int x, int y)
{
    uint32_t* fb = (uint32_t*)_fb_info->framebuffer;
    uint32_t pitch_pixels = _fb_info->pitch / 4;

    return fb[y * pitch_pixels + x];
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
        }// } else if (cursor_y > 0)
        // {
        //     cursor_y -= FONT_HEIGHT;
        //     cursor_x = _fb_info->width - 1;
        // }
        draw_glyph(' ', cursor_x, cursor_y, 0xFFFFFFFF, 0x00000000);
    } else
    {
        draw_glyph(c, cursor_x, cursor_y, 0xCCCCCCCC, 0x00000000);
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

void vga_print_hex(uint32_t n)
{
    char hex_chars[] = "0123456789ABCDEF";
    char buffer[11]; // "0x" + 8 digits + null terminator

    buffer[0] = '0';
    buffer[1] = 'x';
    
    // Process each 4-bit nibble from left to right
    for (int i = 7; i >= 0; i--)
    {
        // Shift the nibble into the lowest 4 bits and mask it
        buffer[i + 2] = hex_chars[(n >> ((7 - i) * 4)) & 0xF];
    }
    
    buffer[10] = '\0';
    vga_print(buffer);
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
    uint32_t* fb = (uint32_t*)_fb_info->framebuffer;
    uint32_t pitch_pixels = _fb_info->pitch / 4;
    for (int y = 0; y < _fb_info->height; y++)
        for (int x = 0; x < _fb_info->width; x++)
            fb[y * pitch_pixels + x] = 0x00000000;
    cursor_x = cursor_y = 0;
}
