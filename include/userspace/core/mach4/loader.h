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
#ifndef LOADER_H
#define LOADER_H

#include <stdint.h>

#define MACH4_MAGIC 0x4D344558
#define MACH4_MAGIC_LEN 4
#define MACH4_VERSION 1

struct mach4_header_t {
    uint32_t magic;        // dd 0x4D344558
    uint8_t  version;      // db 1
    uint8_t  flags;        // db 0
    uint32_t entry_offset; // dd _start - $$
    uint32_t reserved1;    // dd 0
    uint32_t reserved2;    // dd 0
    uint16_t reserved3;    // dw 0
} __attribute__((packed));

// struct mach4_header_t {
//     uint32_t magic;               // "M4EX"
//     uint8_t version;                // 1
//     uint8_t flags;                  // Reserved for now

//     uint32_t entry_offset;          // Where the _start label is in our code. Can be changed if needed
//     uint32_t fsize;                 // How big the file is
//     uint32_t permissions;           // Is it admin-only writeable, can the user run it?
//     uint16_t owner;                 // Who owns this?
// }__attribute__((packed));

uint32_t mach4_load(const char* path, uint32_t addr);
void mach4_execute(uint32_t id);

#endif      // LOADER_H
