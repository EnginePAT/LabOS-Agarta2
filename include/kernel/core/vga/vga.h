#ifndef VGA_H
#define VGA_H

#include <stdint.h>
#include <util/util.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

extern uint16_t* video;

void vga_clear(void);
void vga_putchar(char c);
void vga_print(String s);

#endif      // VGA_H
