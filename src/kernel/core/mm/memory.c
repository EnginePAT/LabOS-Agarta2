#include <kernel/core/mm/memory.h>

typedef struct block_header {
    size_t size;
    int free;
    struct block_header* next;
} block_header_t;

#define HEADER_SIZE sizeof(block_header_t)

static block_header_t* heap_start = 0;

void heap_init(uint32_t start, uint32_t size)
{
    heap_start = (block_header_t*)start;
    heap_start->size = size;
    heap_start->free = 1;
    heap_start->next = 0;
}

void* kmalloc(size_t size)
{
    block_header_t* curr = heap_start;
    while (curr)
    {
        if (curr->free && curr->size >= size)
        {
            // Split block if there's enough room
            if (curr->size >= size + HEADER_SIZE + 4)
            {
                block_header_t* new = (block_header_t*)((uint8_t*)curr + HEADER_SIZE + size);
                new->size = curr->size - size - HEADER_SIZE;
                new->free = 1;
                new->next = curr->next;
                curr->next = new;
                curr->size = size;
            }
            curr->free = 0;
            return (void*)((uint8_t*)curr + HEADER_SIZE);
        }
        curr = curr->next;
    }
    return 0;   // out of heap
}

void kfree(void *ptr)
{
    if (!ptr) return;
    block_header_t* header = (block_header_t*)((uint8_t*)ptr - HEADER_SIZE);
    header->free = 1;

    // Coalesce adjacent free blocks
    block_header_t* curr = heap_start;
    while (curr && curr->next)
    {
        if (curr->free && curr->next->free)
        {
            curr->size += HEADER_SIZE + curr->next->size;
            curr->next = curr->next->next;
        } else {
            curr = curr->next;
        }
    }
}
