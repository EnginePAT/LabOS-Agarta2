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
#include <util/util.h>

void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

uint16_t inw(uint16_t port) {
    uint16_t ret;
    __asm__ volatile ("inw %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

int strcmp(const char* a, const char* b)
{
    while (*a && (*a == *b)) {
        a++;
        b++;
    }
    return *(const unsigned char*)a - *(const unsigned char*)b;
}

char* substring(char* buffer, int start, int end)
{
    static char result[128];
    int pos = 0;

    for (int i = start; i <= end; i++)
    {
        result[pos++] = buffer[i];
    }
    result[pos] = '\0';
    return result;
}

int atoi(char* str)
{
    int res = 0;
    for (int i = 0; str[i] != '\n' && str[i] != '\r' && str[i] != '\0'; ++i)
    {
        res = res * 10 + str[i] - '0';
    }
    return res;
}
