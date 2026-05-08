/*
 * LabOS Agarta
 * Copyright (c) 2026 EnginePAT
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */
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
