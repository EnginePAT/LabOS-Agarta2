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
#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

void* kmalloc(size_t size);
void kfree(void* ptr);
void* realloc(void* ptr, size_t size);

void heap_init(uint32_t start, uint32_t size);

#endif      // MEMORY_H
