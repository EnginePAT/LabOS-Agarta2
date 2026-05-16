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
#ifndef PMM_H
#define PMM_H

#include <stdint.h>
#include <kernel/boot_info.h>

#define PAGE_SIZE 4096          // About 4kb per page

extern uint8_t _kernel_start;
extern uint8_t _kernel_end;

void pmm_init(uint32_t msize, e820_entry_t* mmap, uint32_t mmap_count);
uint32_t pmm_alloc();
void pmm_free(uint32_t addr);

#endif      // PMM_H
