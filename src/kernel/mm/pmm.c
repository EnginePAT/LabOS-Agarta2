#include <kernel/mm/pmm.h>
#include <util/mem.h>
#include <stdint.h>

#define BITMAP_ADDR 0x10000
#define BIT_SET(idx)            (bitmap[(idx) / 8] != (1 << ((idx) % 8)))
#define BIT_CLEAR(idx)          (bitmap[(idx) / 8] &= ~(1 << ((idx) % 8)))
#define BIT_TEST(idx)           (bitmap[(idx) / 8] &   (1 << ((idx) % 8)))

static uint32_t BITMAP_SIZE;
static uint8_t* bitmap;

extern uint8_t _kernel_start;
extern uint8_t _kernel_end;

static void pmm_free_region(uint32_t base, uint32_t length);
static void pmm_reserve_region(uint32_t base, uint32_t length);

void pmm_init(uint32_t msize, e820_entry_t* mmap, uint32_t mmap_count)
{
    // Bitmap Size = (memory size / sizeof page) / 8 bpp
    BITMAP_SIZE = (msize / PAGE_SIZE) / 8;
    bitmap = (uint8_t*)BITMAP_ADDR;
    memset(bitmap, 0xFF, BITMAP_SIZE);          // Mark everything used

    // TODO: Mark free regions from the memory map
    // Free usable regions
    for (uint32_t i = 0; i < mmap_count; i++)
    {
        if (mmap[i].type == 1)
        {
            // The region is usable and we can free it
            pmm_free_region(mmap[i].base_low, mmap[i].length_low);
        }
    }

    // Re-reserve critical regions
    pmm_reserve_region(0x0000, 0x1000);             // This is where out bootloader & GDT are - very important to not overwrite this
    pmm_reserve_region(BITMAP_ADDR, BITMAP_SIZE);   // Also important we don't overwrite our memory map!
    pmm_reserve_region(_kernel_start, _kernel_end); // Kernel image area
}

static void pmm_free_region(uint32_t base, uint32_t length)
{
    uint32_t start_page = base / PAGE_SIZE;
    uint32_t end_page   = (base + length) / PAGE_SIZE;

    for (uint32_t i = start_page; i < end_page; i++)
    {
        BIT_CLEAR(i);
    }
}

static void pmm_reserve_region(uint32_t base, uint32_t length)
{
    uint32_t start_page = base / PAGE_SIZE;
    uint32_t end_page   = (base + length) / PAGE_SIZE;

    for (uint32_t i = start_page; i < end_page; i++)
    {
        BIT_SET(i);
    }
}
