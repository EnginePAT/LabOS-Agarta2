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
#include <userspace/core/handlers/handlers.h>
#include <kernel/core/vga/vga.h>
#include <stdint.h>

int sys_exit_handler(uint32_t code, uint32_t _, uint32_t __, uint32_t ___, uint32_t ____)
{
    // Kill process later
    vga_print("sys_exit called\n");
    for(;;);
}

int sys_write_handler(uint32_t fd, uint32_t buf, uint32_t len, uint32_t _, uint32_t __)
{
    // For now, only support fd=1 (stdout)
    if (fd != 1) return -1;

    const char* s = (const char*)buf;
    for (uint32_t i = 0; i < len; i++)
    {
        vga_putchar(s[i]);
    }
    return (int)len;
}
