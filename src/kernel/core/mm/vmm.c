#include <kernel/core/mm/vmm.h>
#include <util/mem.h>
#include <kernel/core/vga/serial.h>
#include <stdint.h>
#include <userspace/userspace.h>

page_dir_t* current_dir;

void vmm_map_page(page_dir_t* dir, uint32_t virt, uint32_t phys, uint32_t flags)
{
    uint32_t dir_idx    = PAGE_DIR_INDEX(virt);
    uint32_t table_idx  = PAGE_TABLE_INDEX(virt);

    // If page table doesn't exist, allocate one
    if (!((*dir)[dir_idx] & PAGE_PRESENT))
    {
        uint32_t table_phys = pmm_alloc();
        // table_phys must be in identity-mapped region or this memset corrupts memory
        memset((void*)table_phys, 0, PAGE_SIZE);
        (*dir)[dir_idx] = table_phys | PAGE_PRESENT | PAGE_WRITEABLE | PAGE_USER;
        //                                                              ^^^ also needs USER flag
        //                                                              for user mappings!
    }

    page_table_t* table = (page_table_t*)PAGE_FRAME((*dir)[dir_idx]);
    (*table)[table_idx] = PAGE_FRAME(phys) | flags | PAGE_PRESENT;
}

void vmm_init(struct LFramebufferInfo* fb_info)
{
    current_dir = (page_dir_t*)pmm_alloc();
    memset(current_dir, 0, PAGE_SIZE);

    // Identity map kernel region
    for (uint32_t addr = 0x1000; addr < (uint32_t)&_kernel_end; addr += PAGE_SIZE)
        vmm_map_page(current_dir, addr, addr, PAGE_PRESENT | PAGE_WRITEABLE);

    // Map framebuffer
    uint32_t fb_start = fb_info->framebuffer;
    uint32_t fb_size  = fb_info->pitch * fb_info->height;
    for (uint32_t addr = fb_start; addr < fb_start + fb_size; addr += PAGE_SIZE)
    {
        vmm_map_page(current_dir, addr, addr, PAGE_PRESENT | PAGE_WRITEABLE);
    }

    // Map heap region (2MB - 3MB)
    for (uint32_t addr = 0x200000; addr < 0x300000; addr += PAGE_SIZE)
    {
        vmm_map_page(current_dir, addr, addr, PAGE_PRESENT | PAGE_WRITEABLE);
    }

    // Map the kernel stack region (e.g., 0x80000 to 0x90000)
    for (uint32_t addr = 0x80000; addr < 0x91000; addr += PAGE_SIZE)
    {
        vmm_map_page(current_dir, addr, addr, PAGE_PRESENT | PAGE_WRITEABLE);
    }

    // Map the user pages & stack
    for (uint32_t addr = 0xBFFFE000; addr < 0xC0000000; addr += PAGE_SIZE)
    {
        vmm_map_page(current_dir, addr, addr,
                    PAGE_WRITEABLE | PAGE_USER);
    }

    serial_print("VMM: fb mapped ");
    serial_print_hex(fb_start);
    serial_print(" size=");
    serial_print_hex(fb_size);
    serial_print("\n");

    asm volatile (
        "mov %0, %%cr3\n"
        "mov %%cr0, %%eax\n"
        "or $0x80000000, %%eax\n"
        "mov %%eax, %%cr0\n"
        :: "r"(current_dir) : "eax"
    );
    serial_print("VMM: paging enabled\n");
}
