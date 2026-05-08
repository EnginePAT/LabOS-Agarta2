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
#include <userspace/core/syscall.h>
#include <kernel/core/vga/vga.h>

int exit(int code)
{
    vga_print("Process exited with code: ");
    vga_print("\n");

    __asm__ volatile (
        "int $0x80"
        :
        : "a"(0), "b"(code)
    );
    for (;;);
    return 0;
}

int write(int fd, const char *buf, unsigned int len)
{
    int ret;
    __asm__ volatile (
        "int $0x80"
        : "=a"(ret)
        : "a"(1), "b"(fd), "c"(buf), "d"(len)
    );
    return ret;
}
