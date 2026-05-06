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
    }

    page_table_t* table = (page_table_t*)PAGE_FRAME((*dir)[dir_idx]);
    (*table)[table_idx] = PAGE_FRAME(phys) | flags | PAGE_PRESENT;
}

void vmm_init(struct LFramebufferInfo* fb_info)
{
    current_dir = (page_dir_t*)pmm_alloc();
    memset(current_dir, 0, PAGE_SIZE);

    // Pre-allocate page tables for identity map (0x00000000 - 0x08000000) = 32 entries
    for (uint32_t dir_idx = 0; dir_idx < 32; dir_idx++) {
        uint32_t table_phys = pmm_alloc();
        memset((void*)table_phys, 0, PAGE_SIZE);
        (*current_dir)[dir_idx] = table_phys | PAGE_PRESENT | PAGE_WRITEABLE;
    }

    // Pre-allocate page table for user stack region (0xBFFFC000 - 0xC0000000)
    // dir_idx for 0xBFFFFFFF = 0x2FF = 767
    {
        uint32_t table_phys = pmm_alloc();
        memset((void*)table_phys, 0, PAGE_SIZE);
        (*current_dir)[767] = table_phys | PAGE_PRESENT | PAGE_WRITEABLE | PAGE_USER;
    }

    // Pre-allocate page table for framebuffer (0xFD000000)
    // dir_idx = 0xFD000000 >> 22 = 0x3F4 = 1012
    {
        uint32_t table_phys = pmm_alloc();
        memset((void*)table_phys, 0, PAGE_SIZE);
        (*current_dir)[1012] = table_phys | PAGE_PRESENT | PAGE_WRITEABLE;
    }

    // Fill identity map PTEs directly
    for (uint32_t addr = 0x1000; addr < 0x08000000; addr += PAGE_SIZE) {
        uint32_t dir_idx   = PAGE_DIR_INDEX(addr);
        uint32_t table_idx = PAGE_TABLE_INDEX(addr);
        page_table_t* table = (page_table_t*)PAGE_FRAME((*current_dir)[dir_idx]);
        (*table)[table_idx] = (addr & ~0xFFF) | PAGE_PRESENT | PAGE_WRITEABLE;
    }

    // Framebuffer — table already exists, vmm_map_page won't call pmm_alloc
    uint32_t fb_start = fb_info->framebuffer;
    uint32_t fb_size  = fb_info->pitch * fb_info->height;
    for (uint32_t addr = fb_start; addr < fb_start + fb_size; addr += PAGE_SIZE)
        vmm_map_page(current_dir, addr, addr, PAGE_PRESENT | PAGE_WRITEABLE);

    // User stack — table already exists, vmm_map_page won't call pmm_alloc
    for (uint32_t i = 0; i < USER_STACK_PAGES; i++) {
        uint32_t phys = pmm_alloc();  // these frames are in identity-mapped region
        uint32_t virt = USER_STACK_TOP - (i + 1) * PAGE_SIZE;
        vmm_map_page(current_dir, virt, phys, PAGE_USER | PAGE_WRITEABLE);
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
