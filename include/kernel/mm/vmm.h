#ifndef VMM_H
#define VMM_H

#include <kernel/boot_info.h>
#include <stdint.h>

#define PAGE_PRESENT            (1 << 0)
#define PAGE_WRITEABLE          (1 << 1)
#define PAGE_USER               (1 << 2)

#define PAGE_DIR_INDEX(va)      ((va) >> 22)
#define PAGE_TABLE_INDEX(va)    (((va) >> 12) & 0x3FF)
#define PAGE_FRAME(pa)          ((pa) & ~0xFFF)

typedef uint32_t page_dir_t[1024];
typedef uint32_t page_table_t[1024];

void vmm_init(struct LFramebufferInfo* fb_info);

#endif      // VMM_H
