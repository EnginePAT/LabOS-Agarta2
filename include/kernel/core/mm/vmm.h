#ifndef VMM_H
#define VMM_H

#include <kernel/boot_info.h>
#include <kernel/core/mm/pmm.h>
#include <stdint.h>

#define PAGE_PRESENT            (1 << 0)
#define PAGE_WRITEABLE          (1 << 1)
#define PAGE_USER               (1 << 2)

#define PAGE_DIR_INDEX(va)      ((va) >> 22)
#define PAGE_TABLE_INDEX(va)    (((va) >> 12) & 0x3FF)
#define PAGE_FRAME(pa)          ((pa) & ~0xFFF)

#define USER_STACK_BASE 0x400000
#define USER_STACK_TOP 0xC0000000
#define USER_CODE_BASE 0x00400000

typedef uint32_t page_dir_t[1024];
typedef uint32_t page_table_t[1024];

extern page_dir_t* current_dir;

void vmm_init(struct LFramebufferInfo* fb_info);
void vmm_map_page(page_dir_t* dir, uint32_t virt, uint32_t phys, uint32_t flags);

#endif      // VMM_H
