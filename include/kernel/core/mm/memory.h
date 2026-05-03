#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

void* kmalloc(size_t size);
void kfree(void* ptr);
void* realloc(void* ptr, size_t size);

void heap_init(uint32_t start, uint32_t size);

#endif      // MEMORY_H
