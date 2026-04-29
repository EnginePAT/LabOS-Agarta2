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
