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
#include <stdint.h>
#include <util/mem.h>

void memset(void *dest, char val, uint32_t count)
{
    char* temp = (char*)dest;
    for (; count != 0; count--)
    {
        *temp++ = val;
    }
}

void memcpy(void* src, void* dest, uint32_t count)
{
    uint8_t* d = (uint8_t*)dest;
    uint8_t* s = (uint8_t*)src;
    for (int i = 0; i < count; i++)
    {
        d[i] = s[i];
    }
}

void kmemcpy(void* dest, void* src, uint32_t count)
{
    uint8_t* d = (uint8_t*)dest;
    uint8_t* s = (uint8_t*)src;
    for (int i = 0; i < count; i++)
    {
        d[i] = s[i];
    }
}
