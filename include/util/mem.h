#ifndef MEM_H
#define MEM_H

#include <stdint.h>

void memset(void* dest, char val, uint32_t count);
void memcpy(void* src, void* dest, uint32_t count);
void kmemcpy(void* dest, void* src, uint32_t count);

#endif      // MEM_H
