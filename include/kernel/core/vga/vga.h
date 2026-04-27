#ifndef VGA_H
#define VGA_H

#include <stdint.h>
#include <util/util.h>
#include <kernel/boot_info.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

extern uint16_t* video;

void setFbInfo(struct LFramebufferInfo* info);

void vga_clear(void);
void vga_putchar(char c);
void vga_print(String s);
void vga_print_hex(uint32_t n);

void fb_print(const char* s, int x, int y, uint32_t fg, uint32_t bg);
void putpixel(int x, int y, uint32_t color);
uint32_t getpixel(int x, int y);

#endif      // VGA_H
