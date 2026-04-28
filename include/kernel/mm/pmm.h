#ifndef PMM_H
#define PMM_H

#include <stdint.h>

#define PAGE_SIZE 4096          // About 4kb per page

void pmm_init(uint32_t msize);

#endif      // PMM_H
