#ifndef PMM_H
#define PMM_H

#include <stdint.h>
#include <kernel/boot_info.h>

#define PAGE_SIZE 4096          // About 4kb per page

void pmm_init(uint32_t msize, e820_entry_t* mmap, uint32_t mmap_count);

#endif      // PMM_H
