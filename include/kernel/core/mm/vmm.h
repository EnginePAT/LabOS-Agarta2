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
#ifndef VMM_H
#define VMM_H

#include <kernel/boot_info.h>
#include <kernel/core/mm/pmm.h>
#include <stdint.h>

#define PAGE_PRESENT            (1 << 0)
#define PAGE_WRITEABLE          (1 << 1)
#define PAGE_USER               (1 << 2)

#define PAGE_DIR_INDEX(va)      ((va) >> 22)
#define PAGE_TABLE_INDEX(va)    (((va) >> 12) & 0x3FF)
#define PAGE_FRAME(pa)          ((pa) & ~0xFFF)

#define USER_STACK_BASE 0x400000
#define USER_STACK_TOP 0xC0000000
#define USER_CODE_BASE 0x00400000
#define USER_STACK_PAGES 4

// typedef uint32_t page_dir_t[1024];
// typedef uint32_t page_table_t[1024];

// extern page_dir_t* current_dir;
extern uint32_t* current_dir;

void vmm_init(struct LFramebufferInfo* fb_info);
void vmm_map_page(uint32_t* dir, uint32_t virt, uint32_t phys, uint32_t flags);
void pmm_reserve_region(uint32_t base, uint32_t length);

#endif      // VMM_H
